#include <iostream>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include <fcntl.h>  
#include <pwd.h>
#include <grp.h>
#include <sstream>
#include <iomanip>
#include <vector>
#include <stack>
#include <unordered_map>
#include <queue>


#define CTRL_KEY(k) ((k) & 0x1f)

using namespace std;

enum keybind {
  ARROW_LEFT = 'a',
  ARROW_RIGHT = 'd',
  ARROW_UP = 'w',
  ARROW_DOWN = 's',
  HOME = 'h',
  QUIT = 'q',
  COLON = ':',
  ESC = '\x1b',
  ENTER = 13,
  BACKSPACE = 127,

};

//File exp struct 
struct explorerConfig{
	int cx,cy;
	int offset;
	int cy_buffer;
	int screenrows;
	int screencols;
	struct termios org_termios;
	bool c_mode;
	char last_key;
	//Flag is set true if shifted from normal to cmd mode 
	bool mode_shift_flag;
	//Flag to refresh in command mode 
	bool refresh_c_mode;
	//String store result of the command mode 
	string c_mode_result;
	string cur_cmd;
};

struct curDirs{
	vector<vector<string>> cur_entities;
	int no_entities;
	int max_entity_len;
	stack<string> dir_loc;
	stack<string> buffer_dir_loc;

};

struct explorerConfig E;
struct curDirs D;

//init
void initDirs(){
	/* Fucntion to initialize Directory variables
	*/
	D.cur_entities.clear();
	D.cur_entities.resize(1000,vector<string>(10));
	D.cur_entities[0][0] = "Entity Name";
	D.cur_entities[0][1] = "Permissions";
	D.cur_entities[0][2] = "Size";
	D.cur_entities[0][3] = "Group";
	D.cur_entities[0][4] = "User";
	D.cur_entities[0][6] = "Date";
	D.no_entities = 0;
}

//helper fuctions 

string PopOneDir(string path){
	/*Function to pop a dir from the given path
	Args: 
        path(string): path of string that needs to be poped
	Returns:
        path(string): updated path
	Eg: 
	Input: /Users/vyshakp/Documents/IIIT/AOS/Assignments/FileExplorer/test
	Gives: /Users/vyshakp/Documents/IIIT/AOS/Assignments/FileExplorer
	*/
	if(path.size()<2||path[0]!='/')
		return path;
	vector<string> loc_details;
    string t;
    stringstream ss(path);
    while (getline(ss, t, '/')){
        loc_details.push_back(t);
    }
    string last_dir_name=loc_details[loc_details.size()-1];
	return path.substr(0,path.size()-last_dir_name.size()-1);
}

string GetHome(){
	/*Function to get the home directory of the current machine
	*/
	const char *homedir;

	if ((homedir = getenv("HOME")) == NULL) {
		homedir = getpwuid(getuid())->pw_dir;
	}
	return homedir;
}

string ResolvePath(string path){
	/*Function to fix the relative path to absolute path
	Args: 
        path(string): path of string that needs to be updated
	Returns:
        path(string): Absolute path
	*/
    switch(path[0]){
        case '/':{
            //here we are expecting the given path is absolute 
			int path_len = path.size();
			if (path_len==1)
            	return path;
			else if(path[path_len-1]=='/')
				return path.substr(0,path_len-1);
			else
				return path;
            break;
			}
        case '~':
            return (GetHome() + path.substr(1));
            break;
        case '.':
            //checking if it is ..
            if(path[1]=='.')
				return PopOneDir(D.dir_loc.top());
            return (D.dir_loc.top() + path.substr(1));
            break;
        default:
			//if we are giving the file name 
            return (D.dir_loc.top()+"/"+path);
            break;
    }

}

// Terminal functions

void Die(const char *s){
	/*Function to display the error message
	Args: 
        s(const char *): Error message that needs to displayed
	*/
	write(STDOUT_FILENO, "\x1b[2J" , 4);
	write(STDOUT_FILENO, "\x1b[H", 3);
	perror(s);
	exit(1);
}

void DisableRawMode(){
	/*Function to disable the raw mode in terminal and loads back the default 
	  flags
	*/
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, & E.org_termios) == -1)
		Die("tcsetattr");
}

void EnableRawMode(){
	/*Function to enable the raw mode in terminal
	*/
	if (tcgetattr(STDIN_FILENO, & E.org_termios) == -1)
		Die("tcsetattr");

	atexit(DisableRawMode);

	struct termios raw = E.org_termios;

	raw.c_iflag &= ~(BRKINT | IXON | ICRNL | INPCK | ISTRIP);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		Die("tcsetattr");
}

int GetCursorPosition(int *rows, int *cols){
	/*Function to get the cursor position
	Args: 
        rows(int *): Address of row variable
        cols(int *): Address of column variable
	Returns:
        stat(int): Status of the fucntion
	*/
	char buff[32];
	int i=0;
	if(write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
		return -1;
	while(i<sizeof(buff)-1){
		if (read(STDIN_FILENO, &buff[i], 1)!=1) break;
		if(buff[i]=='R') break;
		i++;
	}
	buff[i] = '\0';
	if(buff[0]!='\x1b'||buff[1]!='[') return -1;
	if(sscanf(&buff[2], "%d;%d", rows, cols)!=2) return -1;
	return 0;

}

int GetWindowSize(int *rows, int *cols){
	/*Function to get the current window size
	Args: 
        rows(int *): Address of row variable
        cols(int *): Address of column variable
	Returns:
        stat(int): Status of the fucntion
	*/
	struct winsize ws;

	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col ==0){
		if(write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
			return -1;
		return GetCursorPosition(rows, cols);

	}
	else{
		*cols = ws.ws_col;
		*rows = ws.ws_row;
		return 0;
	}
}

// Output Functions 

string  GetPremission(mode_t perm_val){
	/*Function to the permission string 
	Args: 
        perm_val(mode_t): Permission flag of the entity 
	Returns:
        permissions(string): Permissions of the entity
	*/
	string permissions = "";
    permissions += (perm_val & S_IRUSR) ? 'r' : '-';
    permissions += (perm_val & S_IWUSR) ? 'w' : '-';
    permissions += (perm_val & S_IXUSR) ? 'x' : '-';
    permissions += (perm_val & S_IRGRP) ? 'r' : '-';
    permissions += (perm_val & S_IWGRP) ? 'w' : '-';
    permissions += (perm_val & S_IXGRP) ? 'x' : '-';
    permissions += (perm_val & S_IROTH) ? 'r' : '-';
    permissions += (perm_val & S_IWOTH) ? 'w' : '-';
    permissions += (perm_val & S_IXOTH) ? 'x' : '-';
    permissions += '\0';
	return permissions;
}

string EpochToLocal(long epoch) {
	/* Function to convert epoch time to local time
	Args: 
        epoch(long): Time in epoch format 
	Returns:
        time(string): Local time format 
	*/
        const time_t old = (time_t)epoch;
        struct tm *oldt = localtime(&old);
		string local_time=asctime(oldt);
		//removing the enline char
        local_time.pop_back();
		return local_time;
}

void GetDirs(const char* dirname){
	/* Function to the entity details in the given directory
	Args: 
        dirname(const char*): Direcotry path
	*/

	DIR* dir=opendir(dirname);
	
    if(dir == NULL)
        return;

    struct dirent* entity;
    struct stat buf;
    entity =readdir(dir);
	int i=1;
	D.max_entity_len=0;

    while(entity){
		//st_mode for permissions 
        //Ref: https://www.mkssoftware.com/docs/man5/struct_stat.5.asp
		//using absolute path to get correct details 
		string abs_name = (string)dirname + "/" + entity->d_name;
        stat(abs_name.c_str(),&buf);
        //user and group owners 
        struct passwd *us = getpwuid(buf.st_uid);
        struct group  *gr = getgrgid(buf.st_gid);
		//type of file https://aljensencprogramming.wordpress.com/tag/st_mode/
        string permissions = GetPremission(buf.st_mode);
		string group_name,user_name;
		if(gr)
        	group_name = gr->gr_name;
		else
			group_name = "---";
		if(us)
        	user_name = us->pw_name;
		else
			user_name = "---";
		string mod_time=EpochToLocal(buf.st_mtime);
		//checking if directory or not
		string is_dir =(S_ISDIR(buf.st_mode))? "true" : "false";
		float size_kb = (float)buf.st_size/1024;

		D.cur_entities[i][0] = entity->d_name;
		D.cur_entities[i][1] = permissions;
		D.cur_entities[i][2] = to_string(size_kb);
		D.cur_entities[i][3] = group_name;
		D.cur_entities[i][4] = user_name;
		D.cur_entities[i][5] = is_dir;
		D.cur_entities[i][6] = mod_time;
		D.max_entity_len=D.max_entity_len>D.cur_entities[i][0].length()?\
						 D.max_entity_len:D.cur_entities[i][0].length();
		i++;
        entity =readdir(dir);
    }
	D.no_entities=i-1;
	//sorting 
	sort(D.cur_entities.begin()+1,D.cur_entities.begin()+D.no_entities+1,\
		 [](vector<string> &a, vector<string> &b){
													return a[0]<b[0];
												  });
    closedir(dir);
}

void SetScreen(string *buffer){
	/* Function to update the current directory details
	Args: 
        buffer(string*): Buffer string that is getting updated
	*/
	int i=E.offset+1;
	stringstream home_row;
	int entity_name_width = max(15,D.max_entity_len+5);
	home_row<<"\033[92m"<<setw(entity_name_width)<<left<<D.cur_entities[0][0]<<setw(15)<<left<<D.cur_entities[0][1];
	home_row<<setw(15)<<left<<D.cur_entities[0][2]<<setw(15)<<left<<D.cur_entities[0][3];
	home_row<<setw(15)<<left<<D.cur_entities[0][4]<<setw(15)<<left<<D.cur_entities[0][6]<<"\033[0m\r\n";
	*buffer+=home_row.str();

	while(i<E.screenrows+E.offset-2){
	stringstream entity_row;
	//not adding \n at the end because the date variable has a \n in it
	if(D.cur_entities[i][5]=="true"){
		entity_row<<"\033[96;1m"<<setw(entity_name_width)<<left<<D.cur_entities[i][0]<<"\033[0m";
		entity_row<<setw(15)<<left<<D.cur_entities[i][1]<<setw(15)<<left<<D.cur_entities[i][2];
		entity_row<<setw(15)<<left<<D.cur_entities[i][3]<<setw(15)<<left<<D.cur_entities[i][4];
		entity_row<<setw(25)<<left<<D.cur_entities[i][6]<<"\r\n";
	}
	else{
		entity_row<<setw(entity_name_width)<<left<<D.cur_entities[i][0]<<setw(15)<<left<<D.cur_entities[i][1];
		entity_row<<setw(15)<<left<<D.cur_entities[i][2]<<setw(15)<<left<<D.cur_entities[i][3];
		entity_row<<setw(15)<<left<<D.cur_entities[i][4]<<setw(25)<<left<<D.cur_entities[i][6]<<"\r\n";
	}
	i++;
	*buffer+=entity_row.str();
	}
}

bool Search(string entity_name){
	/* Function to search the given entity in current directory
	Args: 
        entity_name(string): Name of the entity that needs to searched
	Returns:
        bool: Status of search 
	*/

	queue<string> to_visit_dir;
    to_visit_dir.push(D.dir_loc.top());
    while(!to_visit_dir.empty()){

		string cur_dir = to_visit_dir.front();
		to_visit_dir.pop();
		DIR* dir=opendir(cur_dir.c_str());

		if(dir == NULL)
			return false;

		struct dirent* entity;
		struct stat buf;
		entity =readdir(dir);
		
		while(entity){
			stat((cur_dir+"/"+entity->d_name).c_str(),&buf);
			int check_cur_dir = strcmp(entity->d_name, ".");
			int check_prev_dir = strcmp(entity->d_name, "..");
			if (entity->d_name == entity_name)
					return true;
			if((S_ISDIR(buf.st_mode))){
				if(check_cur_dir!=0&&check_prev_dir!=0)
				{    
					string new_loc = cur_dir+"/"+entity->d_name;
					to_visit_dir.push(new_loc);
				}
			}
			entity =readdir(dir);
		}
		closedir(dir);
    }

    return false;
}
		
void Goto(string dest_loc){
	/* Function to goto to given destination location
	Args: 
        dest_loc(string): Destination location
	*/
	D.dir_loc.push(ResolvePath(dest_loc));

}

void CreateDir(string dir_path, mode_t permission=(S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)){
	/* Function to create a directory
	Args: 
        dir_path(string): Destination location for the directory
        permission(mode_t): Permission flag for the directory
	*/
	mkdir(dir_path.c_str(), permission);

}

void CreateFile(string file_loc){
	/* Function to create a file
	Args: 
        file_loc(string): Destination location for the file
	*/
	ofstream {file_loc};
}

void DeleteFile(string file_loc){
	/* Function to deelte a file
	Args: 
        file_loc(string): Location of the file
	*/
	if(remove(file_loc.c_str()) != 0 )
		Die("Delete File");
}

void CopyFile(string src_loc, string dst_loc){
	/* Function to copy a file
	Args: 
        src_loc(string): Source location of the file
        dst_loc(string): Destination location of the file
	*/

    ifstream  src(src_loc, ios::binary);
    ofstream  dst(dst_loc,  ios::binary);
    dst << src.rdbuf();

    struct stat buf;
    stat(src_loc.c_str(),&buf);
	if(chmod(dst_loc.c_str(), buf.st_mode) != 0 )
		Die("Chmod of File");
}

void CopyDir(string src_dir, string dest_loc){
	/* Function to copy a directory
	Args: 
        src_dir(string): Source location of the directory
        dest_loc(string): Destination location of the directory
	*/
    
    queue<string> to_visit_dir;
    to_visit_dir.push(src_dir);
    //maintaining a vector to keep track of the insertion order
    //unordered map will contain the folders and the corresponding files
    //here we are traversing in BFS order
    vector<string> dirs_present;
    unordered_map<string,vector<string>> dir_details;
    
    while(!to_visit_dir.empty()){
        vector<string> cur_dir_files;
        string cur_dir = to_visit_dir.front();
        
        dirs_present.push_back(cur_dir);
        to_visit_dir.pop();
        DIR* dir=opendir(cur_dir.c_str());
        if(dir == NULL)
            return;

        struct dirent* entity;
        struct stat buf;
        entity =readdir(dir);
        while(entity){
            stat((cur_dir+"/"+entity->d_name).c_str(),&buf);
            int check_cur_dir = strcmp(entity->d_name, ".");
            int check_prev_dir = strcmp(entity->d_name, "..");
            if((S_ISDIR(buf.st_mode))){
                if(check_cur_dir!=0&&check_prev_dir!=0)
                {    
                    string new_loc = cur_dir+"/"+entity->d_name;
                    to_visit_dir.push(new_loc);
                }
            }
            else{
                string file_loc = entity->d_name;
                cur_dir_files.push_back(file_loc);
            }
            
                
            entity =readdir(dir);
        }
        dir_details.insert({cur_dir,cur_dir_files});
        closedir(dir);
    }

    //geting the folder name to be copied
    //Example:
	//Getting copy_from folder name from here /Users/vyshakp/copy_from
    vector<string> loc_details;
    string t;
    stringstream ss(src_dir);
    while (getline(ss, t, '/')){
        loc_details.push_back(t);
    }

    string dir_name=loc_details[loc_details.size()-1];
    int remaining_path_size = src_dir.size()-dir_name.size();

    for(auto &dir:dirs_present){
        string relative_path = dir.substr(remaining_path_size);

        struct stat buf;
        stat(dir.c_str(),&buf);
        CreateDir(dest_loc+"/"+relative_path, buf.st_mode);

        for(auto &file:dir_details[dir]){
            string src_file_path = dir+"/"+file;
            string dest_file_path = dest_loc+"/"+relative_path+"/"+file;
            CopyFile(src_file_path,dest_file_path);
        }
    } 
    
}

void DeleteDir(string src_dir){
	/* Function to delete a directory
	Args: 
        src_dir(string): Source location of the directory
	*/

    queue<string> to_visit_dir;
    to_visit_dir.push(src_dir);
    //maintaining a vector to keep track of the insertion order
    //unordered map will contain the folders and the corresponding files
    //here we are traversing in BFS order
    vector<string> dirs_present;
    unordered_map<string,vector<string>> dir_details;
    
    while(!to_visit_dir.empty()){
        vector<string> cur_dir_files;
        string cur_dir = to_visit_dir.front();
        
        dirs_present.push_back(cur_dir);
        to_visit_dir.pop();
        DIR* dir=opendir(cur_dir.c_str());
        if(dir == NULL)
            return;

        struct dirent* entity;
        struct stat buf;
        entity =readdir(dir);
        while(entity){
            stat((cur_dir+"/"+entity->d_name).c_str(),&buf);
            int check_cur_dir = strcmp(entity->d_name, ".");
            int check_prev_dir = strcmp(entity->d_name, "..");
            if((S_ISDIR(buf.st_mode))){
                if(check_cur_dir!=0&&check_prev_dir!=0)
                {    
                    string new_loc = cur_dir+"/"+entity->d_name;
                    to_visit_dir.push(new_loc);
                }
            }
            else{
                string file_loc = entity->d_name;
                cur_dir_files.push_back(file_loc);
            }
            entity =readdir(dir);
        }
        dir_details.insert({cur_dir,cur_dir_files});
        closedir(dir);
    }
	//Reversing the vector for deletion operation
    reverse(dirs_present.begin(),dirs_present.end());
    for(auto &dir:dirs_present){

        for(auto &file:dir_details[dir]){
            string src_file_path = dir+"/"+file;
            DeleteFile(src_file_path);
        }
        DeleteFile(dir);
    }
}

void MoveDir(string src_dir, string dest_loc){
	/* Function to move a directory
	Args: 
        src_dir(string): Source location of the directory
        dest_loc(string): Destination location of the directory
	*/

    CopyDir(src_dir,dest_loc);
    DeleteDir(src_dir);
}

void ExecCommand(){
	/* Function to execute a command in Command mode
	*/

	string cmd = E.cur_cmd;
	int len = cmd.size();

	if(cmd.substr(0,4)=="copy"){

		vector<string> loc_details;
		string file;
		string dest_dir,dest_loc,src_loc;
		stringstream ss(cmd.substr(5));
		while (getline(ss, file, ' ')){
			loc_details.push_back(file);
		}
		if(loc_details.size()<2){
				E.refresh_c_mode=true;	
				E.c_mode_result="\033[91mNot a valid Input\033[0m";	
				return;
			}
		dest_dir=loc_details[loc_details.size()-1];
		dest_dir=ResolvePath(dest_dir);
		loc_details.pop_back();
        struct stat buf;
		for(string file:loc_details){
			src_loc = ResolvePath(file);

			if(open(src_loc.c_str(),O_RDONLY)==-1){
				E.refresh_c_mode=true;	
				E.c_mode_result="\033[91mInvalid path\033[0m";	
				return;
			}

			stat(src_loc.c_str(),&buf);
			if((S_ISDIR(buf.st_mode))){
				dest_loc=dest_dir;
				CopyDir(src_loc,dest_loc);
			}
			else{
				dest_loc=dest_dir+"/"+file;
				CopyFile(src_loc,dest_loc);
			}
		}
		E.refresh_c_mode=true;	
		E.c_mode_result="\033[93mCopy Completed\033[0m";	
	}

	else if(cmd.substr(0,4)=="move"){

		vector<string> loc_details;
		string file;
		string dest_dir,dest_loc,src_loc;
		stringstream ss(cmd.substr(5));
		while (getline(ss, file, ' ')){
			loc_details.push_back(file);
		}
		if(loc_details.size()<2){
				E.refresh_c_mode=true;	
				E.c_mode_result="\033[91mNot a valid Input\033[0m";	
				return;
			}
		dest_dir=ResolvePath(loc_details[loc_details.size()-1]);
		loc_details.pop_back();

        struct stat buf;
		for(string file:loc_details){
			src_loc = ResolvePath(file);

			if(open(src_loc.c_str(),O_RDONLY)==-1){
				E.refresh_c_mode=true;	
				E.c_mode_result="\033[91mInvalid path\033[0m";	
				return;
			}

			stat(src_loc.c_str(),&buf);
			if((S_ISDIR(buf.st_mode))){
				dest_loc=dest_dir;
				CopyDir(src_loc,dest_loc);
				DeleteDir(src_loc);
			}
			else{
				dest_loc=dest_dir+"/"+file;
				CopyFile(src_loc,dest_loc);
				DeleteFile(src_loc);
			}
		}
		E.refresh_c_mode=true;
		E.c_mode_result="\033[93mMove Completed\033[0m";
	}

	else if(cmd.substr(0,6)=="rename"){

		vector<string> loc_details;
		string file;
		string new_name,old_name,src_dir;
		stringstream ss(cmd.substr(7));
		while (getline(ss, file, ' ')){
			loc_details.push_back(file);
		}
		if(loc_details.size()!=2){
				E.refresh_c_mode=true;	
				E.c_mode_result="\033[91mNot a valid Input\033[0m";	
				return;
			}
		old_name=ResolvePath(loc_details[0]);
		new_name=ResolvePath(loc_details[1]);
		if(open(old_name.c_str(),O_RDONLY)==-1){
				E.refresh_c_mode=true;	
				E.c_mode_result="\033[91mInvalid File Name\033[0m";	
				return;
		}
		CopyFile(old_name,new_name);
		DeleteFile(old_name);
		E.refresh_c_mode=true;
		E.c_mode_result="\033[93mRename Done\033[0m";
	}

	else if(cmd.substr(0,11)=="create_file"){

		vector<string> loc_details;
		string file;
		string dest_loc,file_name;
		stringstream ss(cmd.substr(12));
		while (getline(ss, file, ' ')){
			loc_details.push_back(file);
		}
		file_name=loc_details[0];
		dest_loc=ResolvePath(loc_details[1]);
		if(open(dest_loc.c_str(),O_RDONLY)==-1){
				E.refresh_c_mode=true;	
				E.c_mode_result="\033[91mInvalid Path\033[0m";	
				return;
		}
		CreateFile(dest_loc+"/"+file_name);
		E.refresh_c_mode=true;
		E.c_mode_result="\033[93mFile created\033[0m";
	}

	else if(cmd.substr(0,11)=="delete_file"){

		string file_loc = ResolvePath(cmd.substr(12));
		if(open(file_loc.c_str(),O_RDONLY)==-1){
				E.refresh_c_mode=true;	
				E.c_mode_result="\033[91mInvalid Path\033[0m";	
				return;
		}
		DeleteFile(file_loc);
		E.refresh_c_mode=true;
		E.c_mode_result="\033[93mFile deleted\033[0m";
	}

	else if(cmd.substr(0,10)=="create_dir"){

		vector<string> loc_details;
		string file;
		string dir_loc,dir_name;
		stringstream ss(cmd.substr(11));
		while (getline(ss, file, ' ')){
			loc_details.push_back(file);
		}
		dir_name=loc_details[0];
		dir_loc=ResolvePath(loc_details[1]);
		if(open(dir_loc.c_str(),O_RDONLY)==-1){
				E.refresh_c_mode=true;	
				E.c_mode_result="\033[91mInvalid Path\033[0m";	
				return;
		}
		string dir_path = dir_loc+"/"+dir_name;
		CreateDir(dir_path);
		E.refresh_c_mode=true;
		E.c_mode_result="\033[93mDirectory created\033[0m";
	}

	else if(cmd.substr(0,10)=="delete_dir"){

		string dir_loc = ResolvePath(cmd.substr(11));
		if(open(dir_loc.c_str(),O_RDONLY)==-1){
				E.refresh_c_mode=true;	
				E.c_mode_result="\033[91mInvalid Path\033[0m";	
				return;
		}
		DeleteDir(dir_loc);
		E.refresh_c_mode=true;
		E.c_mode_result="\033[93mDirectory deleted\033[0m";
	}

	else if(cmd.substr(0,4)=="goto"){

		string dest_loc = ResolvePath(cmd.substr(5));
		if(open(dest_loc.c_str(),O_RDONLY)==-1){
				E.refresh_c_mode=true;	
				E.c_mode_result="\033[91mInvalid Path\033[0m";	
				return;
		}
		Goto(dest_loc+"/");
		E.refresh_c_mode=true;
	}

	else if(cmd.substr(0,6)=="search"){

		string name = cmd.substr(7);
		if (Search(name))
			E.c_mode_result="\033[92mTrue\033[0m";
		else
			E.c_mode_result="\033[91mFalse\033[0";
		E.refresh_c_mode=true;
	}
	else if(cmd.substr(0,4)=="quit"){

		write(STDOUT_FILENO, "\x1b[2J" , 4);
		write(STDOUT_FILENO, "\x1b[H", 3);
		exit(0);
	}
	
	E.cur_cmd="";
}

void SetScreenFooter(string *buffer){
	/* Function to set the footer of the present screen
	Args: 
        buffer(string*): Source location of the directory
	*/

	string cur_mode = E.c_mode ? " Command mode :":" Normal Mode :";
	cur_mode = "\033[30;47m"+cur_mode+"\033[0m";
	if (!E.c_mode)
		*buffer+= cur_mode + D.dir_loc.top();
	else{
			if (E.last_key!=ENTER){

				if(E.last_key==BACKSPACE){
					if(!E.cur_cmd.empty())
						E.cur_cmd.pop_back();
				}

				else if(!E.mode_shift_flag){
					E.cur_cmd+=E.last_key;
					string cmd = E.cur_cmd;
					int len = cmd.size();
				}
					
				else if (E.mode_shift_flag){
					E.cur_cmd="";
					E.mode_shift_flag=false;
				}	

			}
			else{
				ExecCommand();
				E.cur_cmd="";
			}
			*buffer+= cur_mode + E.cur_cmd;	
			if(E.c_mode_result != ""){
				*buffer+=("\r\n"+E.c_mode_result);
			}	
		}
}

void RefreshScreen(){
	/* Function to Refresh screen with the current directory details
	*/

	//\x1b[?25l and \x1b[?25h is to hide cursor before refreshing  
	string refreshScreenBuffer = "\x1b[?25l";
	//\x1b[2J is to clear the screen 
	//\x1b[H is to bring the cursor back to begining 
	refreshScreenBuffer+="\x1b[2J\x1b[3J";
	refreshScreenBuffer+="\x1b[H";
	const char* loc = D.dir_loc.top().c_str();
	
	//Getting the new directory details
	GetDirs(loc);
	SetScreen(&refreshScreenBuffer);
	SetScreenFooter(&refreshScreenBuffer);
	

	// setting the cursor position
	stringstream cursor_pos;
	cursor_pos<<"\x1b["<<E.cy + 1<<";"<<E.cx + 1<<"H";
	refreshScreenBuffer+=cursor_pos.str();
	refreshScreenBuffer+="\x1b[?25h";

	write(STDOUT_FILENO, refreshScreenBuffer.c_str(), refreshScreenBuffer.size());
}

// Input Functions

char ReadKey(){
	/* Function to read one key press in terminal and to refresh the screen size
	   if terminal size is changed.
	*/
	int nkeys;
	char c;
	int cur_screenrows = E.screenrows;
	int cur_screencols = E.screencols;

	//To refresh screen for command mode fuctions 
	if (E.refresh_c_mode){
		initDirs();
		RefreshScreen();
		E.refresh_c_mode=false;
		return char(0);
	}
			
	while((nkeys = read(STDIN_FILENO, &c, 1)) != 1){
		if(nkeys == -1 && errno != EAGAIN) Die("read"); 

		if(GetWindowSize(&E.screenrows, &E.screencols) == -1 )
		Die("getWindowSize");
		if(cur_screenrows!=E.screenrows|| cur_screencols!=E.screencols){
			RefreshScreen();
			cur_screenrows = E.screenrows;
			cur_screencols = E.screencols;
		}
		
	}
	
	//For getting the last pressed key in Command mode 
	if (E.c_mode)
		E.last_key = c;

	//To display the command mode execution message 
	if(E.c_mode_result != "")
		E.c_mode_result="";

	//For normal mode 
	char key[2];
	if (c==27){
		if (read(STDIN_FILENO, &key[0], 1) != 1) return '\x1b';
    	if (read(STDIN_FILENO, &key[1], 1) != 1) return '\x1b';
		if(key[0]=='['){
			switch(key[1]){
				case 'A': return ARROW_UP;
				case 'B': return ARROW_DOWN;
				case 'C': return ARROW_RIGHT;
				case 'D': return ARROW_LEFT;
				case 'H': return HOME;
			}
		}
	}
	return c;
}

void ProcessKeyPress(){
	/* Function to read the key press and perform the required function
	*/

	char c;
	c = ReadKey();

	//Keys available for Normal mode functions
	if(!E.c_mode){
		switch(c){
			
		case ARROW_UP:
			if(E.cy>1)
				E.cy--;
			else
				if(E.offset>0) E.offset--;
			break;

		case ARROW_DOWN:
			if(E.cy<E.screenrows-3 && E.cy<D.no_entities)
				E.cy++;
			else if (E.cy<E.screenrows-3)
				E.cy_buffer++;
			else if(E.cy+E.offset+E.cy_buffer<D.no_entities)
				E.offset++;
			break;
		
		case ARROW_RIGHT:
			if(!D.buffer_dir_loc.empty()){
					initDirs();
					D.dir_loc.push(D.buffer_dir_loc.top());
					D.buffer_dir_loc.pop();
					E.offset=0;
					E.cy_buffer=0;
				}
			break;
		
		case ARROW_LEFT:
			if(!D.dir_loc.empty()){
					initDirs();
					D.buffer_dir_loc.push(D.dir_loc.top());
					D.dir_loc.pop();
					E.offset=0;
					E.cy_buffer=0;
				}
			if(D.dir_loc.empty()) D.dir_loc.push(GetHome());
			break;
		
		case HOME:
			initDirs();
			D.dir_loc.push(GetHome());
			E.offset=0;
			E.cy_buffer=0;
			break;

		case QUIT:
			write(STDOUT_FILENO, "\x1b[2J" , 4);
			write(STDOUT_FILENO, "\x1b[H", 3);
			exit(0);
			break;

		case ENTER:{
			int pos = E.offset+E.cy;
			string name = D.cur_entities[pos][0];
			if (D.cur_entities[pos][5]=="true"){
				initDirs();
				E.cx=0;
				E.cy=1;
				if(name==".");
				else if(name=="..")
					D.dir_loc.push(PopOneDir(D.dir_loc.top()));
				else if(D.dir_loc.top()=="/")
					D.dir_loc.push(D.dir_loc.top()+name);
				else
					D.dir_loc.push(D.dir_loc.top()+"/"+name);
				E.offset=0;
				E.cy_buffer=0;
			}
			else{
				string temp = D.dir_loc.top()+"/"+name;
				int pid=fork();
				if(pid==0){
					execl("/usr/bin/vi", "vi", temp.c_str(), (char*)0);
					exit(1);
				}
				else{
					int wait,stat=0;
					while((wait=waitpid(pid,&stat,0))!=pid)
						if(wait==-1)
							exit(1);
				}

			}
			break;
		}

		case BACKSPACE:{
			string temp,user_dir,home_dir = GetHome();
			stringstream ss (home_dir);
			while (getline (ss, temp, '/'));
			user_dir=home_dir.substr(0,home_dir.size()-temp.size()-1);

			if(D.dir_loc.top()==home_dir){
				initDirs();
				D.buffer_dir_loc.push(home_dir);
				D.dir_loc.pop();
				D.dir_loc.push(user_dir);
				E.offset=0;
				E.cy_buffer=0;
			}
			else if(D.dir_loc.top()==user_dir){
				initDirs();
				D.buffer_dir_loc.push(user_dir);
				D.dir_loc.pop();
				D.dir_loc.push("/");
				E.offset=0;
				E.cy_buffer=0;
			}
			else if (D.dir_loc.top()=="/"){
				initDirs();
			}
			else if(!D.dir_loc.empty()){
				initDirs();
				D.buffer_dir_loc.push(D.dir_loc.top());
				D.dir_loc.pop();
				E.offset=0;
				E.cy_buffer=0;
			}
			break;
		}

		case COLON:
			E.c_mode = true;
			E.mode_shift_flag=true;
			break;
		}

	}

	//Keys available for Command mode functions
	else{
		switch(c){
			case ESC:
				E.c_mode=false;
				break;
		}
	}
}

void initExplorer(){
	/* Function to initialize the File explorer default values
	*/

	E.cx=0;
	E.cy=1;
	E.offset=0;
	E.cy_buffer=0;
	E.c_mode= false;
	E.cur_cmd="";
	E.mode_shift_flag = false;
	E.refresh_c_mode = false;
	E.c_mode_result="";
	D.dir_loc.push(GetHome());
	if(GetWindowSize(&E.screenrows, &E.screencols) == -1 )
		Die("getWindowSize");

}

int main(){
	/* Main funciton that enables the raw mode and wait for the 
	   key press and refresh the screen after processing the key
	   presses
	*/

	EnableRawMode();
	initExplorer();
	initDirs();

	while (true) {
		RefreshScreen();
		ProcessKeyPress();

	}
	return 0;
}
