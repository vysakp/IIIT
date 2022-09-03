#include<iostream>
#include<ctype.h>
#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/ioctl.h>
#include <termios.h>
#include<unistd.h>
#include<string>
#include<dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <sstream>
#include <iomanip>
#include<vector>
#include<stack>
#include <fstream>
#include<queue>
#include<unordered_map>
#include <fcntl.h>  

//remove 
#include <chrono>
#include <thread>

#define CTRL_KEY(k) ((k) & 0x1f)

using namespace std;


// enum mode = { "Normal" , "Command" };

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
	int screenrows;
	int screencols;
	struct termios org_termios;
	bool c_mode;
	char last_key;
	//flag is set true if shifted from normal to cmd mode 
	bool mode_shift_flag;
	//flag to refresh in command mode 
	bool refresh_c_mode;
	//to store result of the command mode 
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
	//make the vector size optimal
	D.cur_entities.clear();
	D.cur_entities.resize(1000,vector<string>(10));
	// vector<string> head_row;
	// head_row.push_back("Entity Name");
	// head_row.push_back("Permissions");
	// head_row.push_back("Size");
	// head_row.push_back("Group");
	// head_row.push_back("user");
	D.cur_entities[0][0] = "Entity Name";
	D.cur_entities[0][1] = "Permissions";
	D.cur_entities[0][2] = "Size";
	D.cur_entities[0][3] = "Group";
	D.cur_entities[0][4] = "user";
	// D.cur_entities[E.screenrows-1][0] = "Normal Mode";
	D.no_entities = 0;
	
	
}


//helper fuctions 

string PopOneDir(string path){
	//Function to pop a dir from the given path
	//Eg : /Users/vyshakp/Documents/IIIT/AOS/Assignments/FileExplorer/test
	// Gives : /Users/vyshakp/Documents/IIIT/AOS/Assignments/FileExplorer
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

string getHome(){
	const char *homedir;

	if ((homedir = getenv("HOME")) == NULL) {
		homedir = getpwuid(getuid())->pw_dir;
	}
	return homedir;
}

string ResolvePath(string path){
    switch(path[0]){
        case '/':
            //here we are expecting the give path is absolute 
            return path;
            break;
        case '~':
            return (getHome() + path.substr(1));
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

// Terminal 

void die(const char *s){
	write(STDOUT_FILENO, "\x1b[2J" , 4);
	write(STDOUT_FILENO, "\x1b[H", 3);
	perror(s);
	exit(1);
}

void disableRawMode(){
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, & E.org_termios) == -1)
		die("tcsetattr");
}

void enableRawMode(){
	if (tcgetattr(STDIN_FILENO, & E.org_termios) == -1)
		die("tcsetattr");

	atexit(disableRawMode);

	struct termios raw = E.org_termios;

	// struct termios raw;
    // tcgetattr(STDIN_FILENO, &raw);
	raw.c_iflag &= ~(BRKINT | IXON | ICRNL | INPCK | ISTRIP);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		die("tcsetattr");
}

int getCursorPosition(int *rows, int *cols){
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
	// printf("\r\n&buff[1]: %s\r\n", &buff[1]);
	return 0;

}

int getwindowSize(int *rows, int *cols){
	struct winsize ws;

	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col ==0){
		if(write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
			return -1;
		return getCursorPosition(rows, cols);

	}
	else{
		*cols = ws.ws_col;
		*rows = ws.ws_row;
		return 0;
	}
}

// Output

string  get_premission(mode_t perm_val){
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

string epoch_to_local(long epoch) {
	/* Fuciton to convert epoch time to local time
	*/
        const time_t old = (time_t)epoch;
        struct tm *oldt = localtime(&old);
        return asctime(oldt);
}





void GetDirs(const char* dirname){
	DIR* dir=opendir(dirname);
	
    if(dir == NULL)
        return;

    struct dirent* entity;
    struct stat buf;
    entity =readdir(dir);
	
	// *buffer+=head_row.str();
	int i=1;
	D.max_entity_len=0;
    while(entity){
		//st_mode for permissions 
        //https://www.mkssoftware.com/docs/man5/struct_stat.5.asp
		//using absolute path to get correct details 
		string abs_name = (string)dirname + "/" + entity->d_name;
        stat(abs_name.c_str(),&buf);
        //user and group owners 
        struct passwd *us = getpwuid(buf.st_uid);
        struct group  *gr = getgrgid(buf.st_gid);


		//type of file https://aljensencprogramming.wordpress.com/tag/st_mode/
        string permissions = get_premission(buf.st_mode);
        string group_name = gr->gr_name;
        string user_name = us->pw_name;
		string mod_time=epoch_to_local(buf.st_mtime);
		//checking if directory or not
		// string is_dir = ((buf.st_mode&S_IFDIR))? "true" : "false";
		string is_dir =(S_ISDIR(buf.st_mode))? "true" : "false";


		float size_kb = (float)buf.st_size/1024;

		//TODO check alternative for to_string


		D.cur_entities[i][0] = entity->d_name;
		D.cur_entities[i][1] = permissions;
		D.cur_entities[i][2] = to_string(size_kb);
		D.cur_entities[i][3] = group_name;
		D.cur_entities[i][4] = user_name;
		D.cur_entities[i][5] = is_dir;
		D.max_entity_len=D.max_entity_len>D.cur_entities[i][0].length()?D.max_entity_len:D.cur_entities[i][0].length();
		i++;
        entity =readdir(dir);
    }
	D.no_entities=i-1;
	//sorting 
	sort(D.cur_entities.begin()+1,D.cur_entities.begin()+D.no_entities+1,[](vector<string> &a, vector<string> &b){
		return a[0]<b[0];
	});
	
    closedir(dir);
}



void setScreen(string *buffer){
	int i=E.offset+1;
	stringstream home_row;
	// entity_row<<"\x1b[K";
	home_row<<setw(D.max_entity_len+5)<<left<<D.cur_entities[0][0]<<setw(15)<<left<<D.cur_entities[0][1];
	home_row<<setw(15)<<left<<D.cur_entities[0][2]<<setw(15)<<left<<D.cur_entities[0][3];
	home_row<<setw(15)<<left<<D.cur_entities[0][4]<<"\r\n";
	*buffer+=home_row.str();
	while(i<E.screenrows+E.offset-2){
	stringstream entity_row;
	// entity_row<<"\x1b[K";
	entity_row<<setw(D.max_entity_len+5)<<left<<D.cur_entities[i][0]<<setw(15)<<left<<D.cur_entities[i][1];
	entity_row<<setw(15)<<left<<D.cur_entities[i][2]<<setw(15)<<left<<D.cur_entities[i][3];
	entity_row<<setw(15)<<left<<D.cur_entities[i][4]<<"\r\n";
	i++;
	*buffer+=entity_row.str();
	}
	
	



}

bool Search(string entity_name){
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
	D.dir_loc.push(dest_loc);

}

void CreateDir(string dir_path, mode_t permission=(S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)){
	// string dir_path = dir_loc+"/"+dir_name;
	mkdir(dir_path.c_str(), permission);

}

void CreateFile(string file_loc){
	//TODO check the permission of the file created
	ofstream {file_loc};
	// write(STDOUT_FILENO, file_loc.c_str(), file_loc.size());
	// std::this_thread::sleep_for(std::chrono::milliseconds(5000)); 
	// creat(file_loc.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	// open(file_loc.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

}

void DeleteFile(string file_loc){
	// int result = remove(file_loc.c_str());
	//TODO delete the file that doesn't exist
	if(remove(file_loc.c_str()) != 0 )
		die("Delete File");
}

void CopyFile(string src_loc, string dst_loc){
    ifstream  src(src_loc, ios::binary);
    ofstream  dst(dst_loc,  ios::binary);
    dst << src.rdbuf();

    struct stat buf;
    stat(src_loc.c_str(),&buf);
    int result =chmod(dst_loc.c_str(), buf.st_mode);
}

void CopyDir(string src_dir, string dest_loc){
    

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
            // cout<<setw(25)<<left<<entity->d_name<<"\n";
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
    //Example: Getting copy_from folder name from here /Users/vyshakp/Documents/IIIT/Temp/copy_from
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
    CopyDir(src_dir,dest_loc);
    DeleteDir(src_dir);
}

void execCommand(){
	string cmd = E.cur_cmd;
	int len = cmd.size();
	//TODO check why first charecter is garbage
	//need to check if refresh is working if copied in the same dir
	if(cmd.substr(0,4)=="copy"){
		// string files = cmd.substr(4);
		//copy
		vector<string> loc_details;
		string file;
		string dest_dir,dest_loc,src_loc;
		stringstream ss(cmd.substr(5));
		while (getline(ss, file, ' ')){
			loc_details.push_back(file);
		}
		dest_dir=loc_details[loc_details.size()-1];
		dest_dir=ResolvePath(dest_dir);
		loc_details.pop_back();
        struct stat buf;
		for(string file:loc_details){
			src_loc = ResolvePath(file);
			
			// dest_loc=dest_dir+"/"+file;
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
		// write(STDOUT_FILENO, files.c_str(), files.size());
		// std::this_thread::sleep_for(std::chrono::milliseconds(5000)); 

		
		
	}
	else if(cmd.substr(0,4)=="move"){
		vector<string> loc_details;
		string file;
		string dest_dir,dest_loc,src_loc;
		stringstream ss(cmd.substr(5));
		while (getline(ss, file, ' ')){
			loc_details.push_back(file);
		}
		dest_dir=ResolvePath(loc_details[loc_details.size()-1]);
		loc_details.pop_back();

        struct stat buf;
		for(string file:loc_details){
			src_loc = ResolvePath(file);

			
			stat(src_loc.c_str(),&buf);
			if((S_ISDIR(buf.st_mode))){
				dest_loc=dest_dir;
				// write(STDOUT_FILENO, src_loc.c_str(), src_loc.size());
				// std::this_thread::sleep_for(std::chrono::milliseconds(5000)); 
				// write(STDOUT_FILENO, dest_loc.c_str(), dest_loc.size());
				// std::this_thread::sleep_for(std::chrono::milliseconds(5000)); 
				CopyDir(src_loc,dest_loc);
				DeleteDir(src_loc);
			}
			else{
				dest_loc=dest_dir+"/"+file;
				// write(STDOUT_FILENO, src_loc.c_str(), src_loc.size());
				// std::this_thread::sleep_for(std::chrono::milliseconds(5000)); 
				// write(STDOUT_FILENO, dest_loc.c_str(), dest_loc.size());
				// std::this_thread::sleep_for(std::chrono::milliseconds(10000)); 
				CopyFile(src_loc,dest_loc);
				DeleteFile(src_loc);
			}

			// dest_loc=dest_dir+"/"+file;
			// write(STDOUT_FILENO, dest_loc.c_str(), dest_loc.size());
			// std::this_thread::sleep_for(std::chrono::milliseconds(5000)); 
			// write(STDOUT_FILENO, dest_loc.c_str(), dest_loc.size());
			// std::this_thread::sleep_for(std::chrono::milliseconds(5000)); 
			// CopyFile(src_loc,dest_loc);
			// DeleteFile(src_loc);
			E.refresh_c_mode=true;
		}
		//copy
	}
	else if(cmd.substr(0,6)=="rename"){
		vector<string> loc_details;
		string file;
		string new_name,old_name,src_dir;
		stringstream ss(cmd.substr(7));
		while (getline(ss, file, ' ')){
			loc_details.push_back(file);
		}
		// old_name=D.dir_loc.top()+"/"+loc_details[0];
		// new_name=D.dir_loc.top()+"/"+loc_details[1];
		old_name=ResolvePath(loc_details[0]);
		new_name=ResolvePath(loc_details[1]);
		CopyFile(old_name,new_name);
		DeleteFile(old_name);
		E.refresh_c_mode=true;
		E.c_mode_result="Rename Done";

		//copy
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
		CreateFile(dest_loc+"/"+file_name);
		E.refresh_c_mode=true;
		E.c_mode_result="File created";

		//copy
	}
	else if(cmd.substr(0,11)=="delete_file"){
		string file_loc = ResolvePath(cmd.substr(12));
		DeleteFile(file_loc);
		E.refresh_c_mode=true;
		E.c_mode_result="File deleted";
		// GetDirs(D.dir_loc.top().c_str());
		//copy
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
		string dir_path = dir_loc+"/"+dir_name;
		CreateDir(dir_path);
		E.refresh_c_mode=true;
		E.c_mode_result="Directory created";
		// GetDirs(D.dir_loc.top().c_str());
		//copy
	}
	else if(cmd.substr(0,4)=="goto"){
		string dest_loc = ResolvePath(cmd.substr(5));
		Goto(dest_loc+"/");
		E.refresh_c_mode=true;
	}

	else if(cmd.substr(0,6)=="search"){
		string name = cmd.substr(7);
		if (Search(name))
			E.c_mode_result="True";
		else
			E.c_mode_result="False";
		E.refresh_c_mode=true;

	}
	else if(cmd=="quit"){
		write(STDOUT_FILENO, "\x1b[2J" , 4);
		write(STDOUT_FILENO, "\x1b[H", 3);
		exit(0);

	}
	
	E.cur_cmd="";
	

}


void setScreenFooter(string *buffer){
	string cur_mode = E.c_mode ? "Command mode:":"Normal Mode:";
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

					//TODO check why first charecter is garbage
					// write(STDOUT_FILENO, to_string(len).c_str(), 10);
					// write(STDOUT_FILENO, cmd.c_str(), 10);
					// std::this_thread::sleep_for(std::chrono::milliseconds(5000)); 
				}
					
				else if (E.mode_shift_flag){
					E.cur_cmd="";
					E.mode_shift_flag=false;
				}
			

			}
			else{
				// E.cur_cmd+=E.last_key;
				execCommand();
				E.cur_cmd="";
				// execCommand();
			}
			*buffer+= cur_mode + E.cur_cmd;	
			if(E.c_mode_result != ""){
				*buffer+=("\r\n"+E.c_mode_result);
			}
			
		}

	}

void RefreshScreen(){
	//string to store refresh screen
	//Check for refreshing the line by line method
	//\x1b[?25l and \x1b[?25h is to hide cursor before refreshing  
	string refreshScreenBuffer = "\x1b[?25l";
	//\x1b[2J is to clear the screen 
	//\x1b[H is to bring the cursor back to begining 
	refreshScreenBuffer+="\x1b[2J";
	refreshScreenBuffer+="\x1b[H";
	

	// write(STDOUT_FILENO, "\x1b[2J" , 4);
	// write(STDOUT_FILENO, "\x1b[H", 3);s
	// DrawRows(&refreshScreenBuffer);
	const char* loc = D.dir_loc.top().c_str();

	
	GetDirs(loc);
	setScreen(&refreshScreenBuffer);
	setScreenFooter(&refreshScreenBuffer);
	

	// setting the cursor position
	stringstream cursor_pos;
	cursor_pos<<"\x1b["<<E.cy + 1<<";"<<E.cx + 1<<"H";
	refreshScreenBuffer+=cursor_pos.str();
	// refreshScreenBuffer+="\x1b[H";
	refreshScreenBuffer+="\x1b[?25h";
	// cout<<*refreshScreenBuffer;
	// cout<<refreshScreenBuffer->size();

	// *buffer+="hello";
	// write(STDOUT_FILENO, buffer->c_str(), buffer->size());
	// write(STDOUT_FILENO, "\x1b[2J" , 4);
	// ProcessKeyPress();
	write(STDOUT_FILENO, refreshScreenBuffer.c_str(), refreshScreenBuffer.size());
}

char ReadKey(){
	int nkeys;
	char c;
	int cur_screenrows = E.screenrows;
	int cur_screencols = E.screencols;

	//to refresh for command mode fuctions 
	//returning HOME as it is invalid in command mode 
	if (E.refresh_c_mode){
		RefreshScreen();
		E.refresh_c_mode=false;
		return char(0);
	}
			
	while((nkeys = read(STDIN_FILENO, &c, 1)) != 1){
		if(nkeys == -1 && errno != EAGAIN) die("read"); 

		if(getwindowSize(&E.screenrows, &E.screencols) == -1 )
		die("getWindowSize");
		if(cur_screenrows!=E.screenrows|| cur_screencols!=E.screencols){
			RefreshScreen();
			cur_screenrows = E.screenrows;
			cur_screencols = E.screencols;
		}
		
	}
	
	//For Command mode 
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








// Input



void ProcessKeyPress(){

	
	char c;
	c = ReadKey();

	// write(STDOUT_FILENO, to_string(c).c_str() , 6);
	// ReadKey();
	if(!E.c_mode){
		switch(c){
			
		case ARROW_UP:
			if(E.cy>1)
				E.cy--;
			else
				if(E.offset>0) E.offset--;
			break;

		case ARROW_DOWN:
		// if(E.cy<D.no_entities)

			// if(E.cy<E.screenrows-2)
			//TODO Fix the screen scrolling 
			if(E.cy<E.screenrows-3 && E.cy<D.no_entities)
					E.cy++;
			else if(E.cy+E.offset<D.no_entities)
				E.offset++;
			
			
			break;
		
		case ARROW_RIGHT:
			if(!D.buffer_dir_loc.empty()){
					initDirs();
					D.dir_loc.push(D.buffer_dir_loc.top());
					D.buffer_dir_loc.pop();
					E.offset=0;
				}
			break;
		
		case ARROW_LEFT:
			if(!D.dir_loc.empty()){
					initDirs();
					D.buffer_dir_loc.push(D.dir_loc.top());
					D.dir_loc.pop();
					E.offset=0;
				}
			if(D.dir_loc.empty()) D.dir_loc.push(getHome());
			break;
		
		case HOME:
			initDirs();
			D.dir_loc.push(getHome());
			E.offset=0;
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
				E.cy=0;
				if(name==".");
				else if(name=="..")
					D.dir_loc.push(PopOneDir(D.dir_loc.top()));
				else
					D.dir_loc.push(D.dir_loc.top()+"/"+name);
				E.offset=0;
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
			string temp,user_dir,home_dir = getHome();
			stringstream ss (home_dir);
			while (getline (ss, temp, '/'));
			user_dir=home_dir.substr(0,home_dir.size()-temp.size()-1);

			if(D.dir_loc.top()==home_dir){
				initDirs();
				D.buffer_dir_loc.push(home_dir);
				D.dir_loc.pop();
				D.dir_loc.push(user_dir);
				E.offset=0;
			}
			else if(D.dir_loc.top()==user_dir){
				initDirs();
				D.buffer_dir_loc.push(user_dir);
				D.dir_loc.pop();
				D.dir_loc.push("/");
				E.offset=0;
			}
			else if (D.dir_loc.top()=="/"){
				initDirs();
			}
			else if(!D.dir_loc.empty()){
				initDirs();
				D.buffer_dir_loc.push(D.dir_loc.top());
				D.dir_loc.pop();
				E.offset=0;
			}
			break;
		}
		case COLON:
			E.c_mode = true;
			E.mode_shift_flag=true;
			//making the last key as null char so that : is not taken 
			break;
		}

	}
	else{
		switch(c){
			case ESC:
				E.c_mode=false;
				break;
			// case BACKSPACE:
			// 	if(!E.cur_cmd.empty())
			// 		E.cur_cmd.pop_back();
			// 	break;



		}
	}
}

//init


void initExplorer(){
	E.cx=0;
	E.cy=1;
	E.offset=0;
	E.c_mode= false;
	E.cur_cmd="";
	E.mode_shift_flag = false;
	E.refresh_c_mode = false;
	E.c_mode_result="";
	// E.last_key= '\0';
	//TODO fix back to home 
	D.dir_loc.push(getHome());
	// D.dir_loc.push("/Users/vyshakp/Documents/IIIT/AOS/Assignments/FileExplorer/test");
	// D.dir_loc.push(".");
	if(getwindowSize(&E.screenrows, &E.screencols) == -1 )
		die("getWindowSize");
}



int main(){
	enableRawMode();
	initExplorer();
	initDirs();
	char key[3];

	while (true) {
		RefreshScreen();
		ProcessKeyPress();
		
		
		// char c='\0';
		// if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
		// if(iscntrl(c))
		// 	printf("%d\r\n",c);
		// else 
		// 	printf("%d('%c')\r\n",c,c);
		// if(c ==CTRL_KEY('c')) 
		// 	break;

	}
	// cout<<"Hello world";
	return 0;
}
