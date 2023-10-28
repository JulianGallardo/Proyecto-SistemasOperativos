#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void removeDir(char *dirName);

//Funcion que borra un directorio en el directorio actual.
int main(int argc, char *argv[]){
    DIR *dir;
    struct dirent *ent;
    if(argc==1){ //Si no se ingreso ningun argumento,no se puede borrar el directorio
        printf("Shell>>No se ingreso el nombre del directorio\n");
    }
    else{//Si se ingreso un argumento, se elimina el directorio.
        if((dir=opendir("."))!=NULL){
            removeDir(argv[1]);
            closedir(dir);
        }
        else{
            perror("");
            return EXIT_FAILURE;
        }
    }

}


void removeDir(char *dirName){//Funcion que elimina un directorio
    if(rmdir(dirName)==-1){
        perror("");
        exit(EXIT_FAILURE);
    }
}

