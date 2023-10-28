#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void createDir(char *dirName);

//Funcion que crea un directorio en el directorio actual.
int main(int argc, char *argv[]){
    DIR *dir;
    struct dirent *ent;
    if(argc==1){ //Si no se ingreso ningun argumento,se crea un directorio con nombre default
        if((dir=opendir("."))!=NULL){
            createDir("newDir");
            closedir(dir);
        }
        else{
            perror("");
            return EXIT_FAILURE;
        }
    }
    else{//Si se ingreso un argumento, se crea un directorio con el nombre ingresado
        if((dir=opendir("."))!=NULL){
            createDir(argv[1]);
            closedir(dir);
        }
        else{
            perror("");
            return EXIT_FAILURE;
        }
    }

}


void createDir(char *dirName){
    if(mkdir(dirName,0777)==-1){
        perror("");
        exit(EXIT_FAILURE);
    }
}

