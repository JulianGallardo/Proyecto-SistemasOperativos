#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

int main(){
    DIR *dir;
    struct dirent *ent;
    if((dir=opendir("."))!=NULL){
        while((ent=readdir(dir))!=NULL) {
            if (ent->d_name[0] != '.') {
                printf("%s\n", ent->d_name);
            }
        }
        closedir(dir);
    }
    else{
        perror("");
        return EXIT_FAILURE;
    }
    return 0;
}

