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

#define CTRL_KEY(k) ((k) & 0x1f)

using namespace std;



//File exp struct 
struct explorerConfig{
	int cx,cy;
	int offset;
	int screenrows;
	int screencols;
	struct termios org_termios;
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
string getHome(){
	const char *homedir;

	if ((homedir = getenv("HOME")) == NULL) {
		homedir = getpwuid(getuid())->pw_dir;
	}
	return homedir;
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

void setScreen(string *buffer){
	int i=E.offset+1;
	stringstream home_row;
	// entity_row<<"\x1b[K";
	home_row<<setw(D.max_entity_len+5)<<left<<D.cur_entities[0][0]<<setw(15)<<left<<D.cur_entities[0][1];
	home_row<<setw(15)<<left<<D.cur_entities[0][2]<<setw(15)<<left<<D.cur_entities[0][3];
	home_row<<setw(15)<<left<<D.cur_entities[0][4]<<"\r\n";
	*buffer+=home_row.str();
	while(i<E.screenrows+E.offset-1){
	stringstream entity_row;
	// entity_row<<"\x1b[K";
	entity_row<<setw(D.max_entity_len+5)<<left<<D.cur_entities[i][0]<<setw(15)<<left<<D.cur_entities[i][1];
	entity_row<<setw(15)<<left<<D.cur_entities[i][2]<<setw(15)<<left<<D.cur_entities[i][3];
	entity_row<<setw(15)<<left<<D.cur_entities[i][4]<<"\r\n";
	i++;
	*buffer+=entity_row.str();
	}
	
	
	*buffer+="Normal mode :" + D.dir_loc.top();
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
	// int status;
	// int key[3] ={-1,-1,-1};
	// while(true){
	// 	if((status=read(STDIN_FILENO, &c, 1))==-1&&errno != EAGAIN) die("read");
	// 	key[2]=c;
	// 	// if(key[2]==61)
	// 	return c;
	// }
	return c;
}




// Input

void ProcessKeyPress(){

	int key[3] = {-1,-1,-1};
	char c;
	while(true){
		// while(!(c = ReadKey())){
		// 	if(getwindowSize(&E.screenrows, &E.screencols) == -1 )
		// 	die("getWindowSize");
		// }

		if(getwindowSize(&E.screenrows, &E.screencols) == -1)
			die("getWindowSize");
		
		c = ReadKey();
		key[2] = c;
		string temp = to_string(c);
		if(key[0]==27 && key[1]==91 ){
			if(key[2]==65){
				// write(STDOUT_FILENO, "up" , 3);
				if(E.cy>0) E.cy--;
				else{
					if(E.offset>0) E.offset--;
				}
				// E.cy=E.cy==0?E.cy:E.cy-1;
				break;
			}
			else if(key[2]==66){
				// write(STDOUT_FILENO, "down" , 6);
				if(E.cy<E.screenrows-2)
					E.cy++;
				else{
					if(E.cy+E.offset<D.no_entities){
						E.offset++;
					}
						
				}

				// E.cy=E.cy==0?E.cy:E.cy+1;
				break;
			}
			else if(key[2]==67){
				// write(STDOUT_FILENO, "right" , 6);
				// if(E.cx<E.screencols)
				// 	E.cx++;
				if(!D.buffer_dir_loc.empty()){
					initDirs();
					D.dir_loc.push(D.buffer_dir_loc.top());
					D.buffer_dir_loc.pop();
					E.offset=0;
				}

				break;
			}
				
			else if(key[2]==68){
				// write(STDOUT_FILENO, "left" , 6);
				// E.cx=E.cx==0?E.cx:E.cx-1;
				// break;
				if(!D.dir_loc.empty()){
					initDirs();
					D.buffer_dir_loc.push(D.dir_loc.top());
					D.dir_loc.pop();
					E.offset=0;
				}
				if(D.dir_loc.empty()) D.dir_loc.push(getHome());
				break;
				}

			else if(key[2]==72){
				initDirs();
				D.dir_loc.push(getHome());
				E.offset=0;
				break;
			}
		}



		if(iscntrl(c)){
			//check for ctrl + M
			if(c==13){
				//Enter
				// write(STDOUT_FILENO, "\x1b[2J" , 4);
				// write(STDOUT_FILENO, "\x1b[H", 3);
				int pos = E.offset+E.cy;
				string name = D.cur_entities[pos][0];
				if (D.cur_entities[pos][5]=="true"){
					initDirs();
					E.cx=0;
					E.cy=0;
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
						while((wait=waitpid(pid,&stat,0))!=pid){
							if(wait==-1) exit(1);
						}
					}

				}
				
				// write(STDOUT_FILENO, "\x1b[2J" , 4);
				// write(STDOUT_FILENO, D.cur_entities[pos][5].c_str() , 10);
				// ProcessKeyPress();
				break;
			}
			else if(c==127){
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

				// if(D.dir_loc.empty()) D.dir_loc.push(getHome());
					
				//Backspace
				break;
			}
		}
			
		if(c=='q') break;

		key[0]=key[1];
		key[1]=key[2];
		// if(getwindowSize(&E.screenrows, &E.screencols) == -1 )
		// die("getWindowSize");
	}
	// char c = ReadKey();
	switch(c){
		case 'q':
			write(STDOUT_FILENO, "\x1b[2J" , 4);
			write(STDOUT_FILENO, "\x1b[H", 3);
			exit(0);
			break;
	}
}

//init


void initExplorer(){
	E.cx=0;
	E.cy=0;
	E.offset=0;
	D.dir_loc.push(getHome());
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
