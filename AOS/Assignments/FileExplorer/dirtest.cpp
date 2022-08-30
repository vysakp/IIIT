#include<iostream>
#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <iomanip>

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

int main(){
    
    listfiles(".");

    return 0;
}
