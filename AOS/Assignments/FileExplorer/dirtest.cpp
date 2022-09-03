#include<iostream>
#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <iomanip>
#include <fstream>
#include<queue>
#include <unordered_map>
#include <sstream>

using namespace std;

void  get_premission(mode_t perm_val, char* permissions){
    permissions[0] = (perm_val & S_IRUSR) ? 'r' : '-';
    permissions[1] = (perm_val & S_IWUSR) ? 'w' : '-';
    permissions[2] = (perm_val & S_IXUSR) ? 'x' : '-';
    permissions[3] = (perm_val & S_IRGRP) ? 'r' : '-';
    permissions[4] = (perm_val & S_IWGRP) ? 'w' : '-';
    permissions[5] = (perm_val & S_IXGRP) ? 'x' : '-';
    permissions[6] = (perm_val & S_IROTH) ? 'r' : '-';
    permissions[7] = (perm_val & S_IWOTH) ? 'w' : '-';
    permissions[8] = (perm_val & S_IXOTH) ? 'x' : '-';
    permissions[9] = '\0';
}

string epoch_to_local(long epoch) {
        // long temp = stol(epoch);
        const time_t old = (time_t)epoch;
        struct tm *oldt = localtime(&old);
        return asctime(oldt);
}


void listfiles(const char* dirname){
    DIR* dir=opendir(dirname);
    if(dir == NULL)
        return;

    struct dirent* entity;
    struct stat buf;
    entity =readdir(dir);
    while(entity){
        //st_mtime last modification
        //size st_size
        //st_mode for permissions 
        //https://www.mkssoftware.com/docs/man5/struct_stat.5.asp
        stat(entity->d_name,&buf);

        //user and group owners 
        struct passwd *us = getpwuid(buf.st_uid);
        struct group  *gr = getgrgid(buf.st_gid);
        //permission
        // char permissions[10];
        // mode_t perm_val = buf.st_mode;
        // permissions[0] = (perm_val & S_IRUSR) ? 'r' : '-';
        // permissions[1] = (perm_val & S_IWUSR) ? 'w' : '-';
        // permissions[2] = (perm_val & S_IXUSR) ? 'x' : '-';
        // permissions[3] = (perm_val & S_IRGRP) ? 'r' : '-';
        // permissions[4] = (perm_val & S_IWGRP) ? 'w' : '-';
        // permissions[5] = (perm_val & S_IXGRP) ? 'x' : '-';
        // permissions[6] = (perm_val & S_IROTH) ? 'r' : '-';
        // permissions[7] = (perm_val & S_IWOTH) ? 'w' : '-';
        // permissions[8] = (perm_val & S_IXOTH) ? 'x' : '-';
        // permissions[9] = '\0';
        char permissions[10];
        get_premission(buf.st_mode, permissions);
        string group_name = gr->gr_name;
        string user_name = us->pw_name;

        float size_kb = (float)buf.st_size/1024;

        

        //printint the st_mode in octal to extract 3 bits 
        string time=epoch_to_local(buf.st_mtime);
        // cout<<typeid(buf.st_mtime).name();
        cout<<setw(25)<<left<<entity->d_name<<setw(12)<<left<<permissions<<setw(12)<<left<<size_kb<<setw(12)<<left<<group_name<<setw(12)<<left<<user_name<<"\n";
        entity =readdir(dir);
    }
    closedir(dir);
}
// if((S_ISDIR(buf.st_mode))){
        //     if(entity->d_name!="."|entity->d_name!="..")
        //     {    string new_loc = (string)dirname+"/"+entity->d_name;
        //         listAllFiles(new_loc.c_str());
        //         }ls
        //     }

bool search_file(const char* dirname, string file_name) {
    queue<string> to_visit_dir;
    to_visit_dir.push(dirname);
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
        //st_mtime last modification
        //size st_size
        //st_mode for permissions 
        //https://www.mkssoftware.com/docs/man5/struct_stat.5.asp
        stat((cur_dir+"/"+entity->d_name).c_str(),&buf);
        int check_cur_dir = strcmp(entity->d_name, ".");
        int check_prev_dir = strcmp(entity->d_name, "..");
        // if(check_cur_dir!=0&&check_prev_dir!=0){
        //     if (entity->d_name == file_name)
        //         return true;
        // }
        // cout<<entity->d_name<<'\n';
        if (entity->d_name == file_name)
                return true;
        cout<<setw(25)<<left<<entity->d_name<<"\n";
        if((S_ISDIR(buf.st_mode))){
            if(check_cur_dir!=0&&check_prev_dir!=0)
            {    
                string new_loc = cur_dir+"/"+entity->d_name;
                to_visit_dir.push(new_loc);
                // cout<<new_loc<<'\n';
                // search_file(new_loc.c_str(),file_name);
            }
                // cout<<setw(25)<<left<<entity->d_name<<"\n";
            }
        // cout<<typeid(buf.st_mtime).name();
        
            
        entity =readdir(dir);
    }
    closedir(dir);
    }

    return false;
}




void copy_file(string src_loc, string dst_loc){
    std::ifstream  src(src_loc, std::ios::binary);
    std::ofstream  dst(dst_loc,   std::ios::binary);
    dst << src.rdbuf();

    struct stat buf;
    stat(src_loc.c_str(),&buf);
    int result =chmod(dst_loc.c_str(), buf.st_mode);
}



void delete_file(string dst_loc){
    int result = remove(dst_loc.c_str());
}
void move_file(string src_loc, string dst_loc){
    copy_file(src_loc,dst_loc);
    delete_file(src_loc);
}

void rename_file(string old_name, string new_name){
    move_file(old_name,new_name);
}
void create_file(string dst_loc){
    std::ofstream {dst_loc};
}
void create_dir(string dir_loc, mode_t permission=(S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)){
    mkdir(dir_loc.c_str(), permission);
}

void copy_dir(string src_dir, string dest_loc){
    

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
                // string file_loc = cur_dir+"/"+entity->d_name;
                string file_loc = entity->d_name;
                cur_dir_files.push_back(file_loc);
            }
            // cout<<typeid(buf.st_mtime).name();
            
                
            entity =readdir(dir);
        }
        dir_details.insert({cur_dir,cur_dir_files});
        closedir(dir);
    }
    // for (auto itr = dirs_present.begin(); itr != dirs_present.end(); ++itr) {
    //     // cout << itr->first<< '\n';
    //     cout <<*itr<<'\n';
    //     for (auto it = dir_details[*itr].begin(); it != dir_details[*itr].end(); ++it)
    //         cout << *it<<" ";
    //     cout<<'\n';
    // }


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
    // cout<<src_dir.substr(0,remaining_path_size);

    for(auto &dir:dirs_present){
        string relative_path = dir.substr(remaining_path_size);
        // cout<<<<"\n";

        struct stat buf;
        
        create_dir(dest_loc+"/"+relative_path, buf.st_mode);

        for(auto &file:dir_details[dir]){
            string src_file_path = dir+"/"+file;
            string dest_file_path = dest_loc+"/"+relative_path+"/"+file;
            copy_file(src_file_path,dest_file_path);
        }
    }
    






    
    
}

void delete_dir(string src_dir){
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
                // string file_loc = cur_dir+"/"+entity->d_name;
                string file_loc = entity->d_name;
                cur_dir_files.push_back(file_loc);
            }
            // cout<<typeid(buf.st_mtime).name();
            
                
            entity =readdir(dir);
        }
        dir_details.insert({cur_dir,cur_dir_files});
        closedir(dir);
    }

    //reversing the order to delete the folders
    reverse(dirs_present.begin(),dirs_present.end());
    for(auto &dir:dirs_present){
        // string relative_path = dir.substr(remaining_path_size);
        // cout<<<<"\n";

        // struct stat buf;
        // stat(dir.c_str(),&buf);
        // create_dir(dest_loc+"/"+relative_path, buf.st_mode);

        for(auto &file:dir_details[dir]){
            string src_file_path = dir+"/"+file;
            // string dest_file_path = dest_loc+"/"+relative_path+"/"+file;
            delete_file(src_file_path);
        }
        // cout<<dir<<"\n";
        delete_file(dir);
    }
    



}

void move_dir(string src_dir, string dest_loc){
    copy_dir(src_dir,dest_loc);
    delete_dir(src_dir);
}
string getHome(){
	return "/Users/vyshakp";
}


// string ResolvePath(string path){
//     switch(path[0]){
//         case '/':
//             //here we are expecting the give path is absolute 
//             return path;
//             break;
//         case '~':
//             return (getHome() + path.substr(1));
//             break;
//         case '.':
//             //checking if it is ..
//             if(path[1]=='.')
//                 return (getHome() + path.substr(2));
//             return (getHome() + path.substr(1));
//             break;
//         default:

//             break;
//     }

// }

int main(){
    string src_loc = "/Users/vyshakp/Documents/IIIT/AOS/Assignments/FileExplorer/test/temp";
    string dst_loc = "/Users/vyshakp/Documents/IIIT/AOS/Assignments/FileExplorer/test/temp";
    string delete_loc = "/Users/vyshakp/Documents/IIIT/Temp/copy_to/copy_from";
    copy_dir(src_loc,dst_loc);
    // listAllFiles(src_loc.c_str());
    // copy_dir(src_loc,dst_loc);
    // move_dir(src_loc,dst_loc);

    // cout<<ResolvePath("~/kjsnfkafjk/fhkjahfjkha");
    // listfiles(".");

    return 0;
}
