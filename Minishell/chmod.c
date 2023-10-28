#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
void changePermissions(char *fileName, char *permissions);

int main(int argc, char *argv[]){

    if(argc==1){
        printf("Shell>>No se ingreso el nombre del archivo ni los permisos a modificar\n");
    }
    else{
        if(argc==2){
            printf("Shell>>No se ingreso los permisos a modificar\n");
        }
        else{//Si se ingreso un nombre y valores, se cambian los permisos del archivo.
            changePermissions(argv[1],argv[2]);
        }
    }
    return 0;
}

void changePermissions(char *fileName, char *permissions){
    unsigned int permissionsInt=0;
    sscanf(permissions,"%o",&permissionsInt);//Cambiamos los permisos a octal

    if(chmod(fileName,permissionsInt)==-1){
        perror("");
        exit(EXIT_FAILURE);
    }
}