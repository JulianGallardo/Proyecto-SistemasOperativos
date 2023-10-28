#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
/*
    a) Mostrar una ayuda con los comandos posibles. H
    b) Crear un directorio. H
    c) Eliminar un directorio. H
    d) Crear un archivo.
    e) Listar el contenido de un directorio. H
    f ) Mostrar el contenido de un archivo.
    g) Modificar los permisos de un archivo. Los permisos son de lectura, escritura y
    ejecución
 */


#define MAX_CMD_ARGV 8
#define MAX_CMD_ARG 16
#define MAX_COMMAND 256

void getCMD(char *cmd);
void parseCMD(char *cmd, char **argv);
void changeDirectory(char *dirName);
void printWorkingDirectory();


int main(){
    char* dirInicial= getcwd(NULL,0);
    char cmd[MAX_COMMAND];
    char** argv;
    argv=(char**)malloc(MAX_CMD_ARGV*sizeof(char*));
    for(int i=0;i<MAX_CMD_ARGV;i++){
        argv[i]=(char*)malloc(MAX_CMD_ARG*sizeof(char));
    }

    pid_t pid;
    while(1){
        getCMD(cmd);//Obtener el comando
        parseCMD(cmd,argv);//Parsear el comando
        if(strcmp(cmd,"exit")==0){ //Si el comando es exit, terminar el programa
            free(argv);
            exit(0);
        }else {
            if (strcmp(cmd, "") == 0) { //Si no se ingreso ningun comando, imprimimos el error
                printf("Shell>>No se ingreso ningun comando\n");
            }
            else if (strcmp(cmd, "cd") == 0) {
                changeDirectory(argv[1]);
                }
            else {
                if (strcmp(cmd, "pwd") == 0) {
                    printWorkingDirectory();
                } else {
                    pid = fork();
                    if (pid == -1) {
                        printf("Shell>>Error al crear el proceso hijo\n");
                        exit(1);
                    }
                    if (pid == 0) {
                        char *comandoAEjecutar = strcat(strcat(dirInicial, "/"), cmd);
                        if (execve(comandoAEjecutar, argv, NULL) == -1) {
                            printf("Shell>>Error al ejecutar el comando\n");
                            exit(1);
                        }
                        exit(0);
                    } else {
                        wait(NULL);
                        }
                    }
                }
        }
    }

    return 0;
}

void getCMD(char *cmd){
    printf("Shell>>");
    fgets(cmd,MAX_COMMAND,stdin);
    cmd[strlen(cmd)-1]='\0';
}

void parseCMD(char *cmd, char **argv){
    char *token;
    int i=0;
    token = strtok(cmd," ");
    while(token!=NULL){
        argv[i]=token;
        i++;
        token=strtok(NULL," ");
    }
    argv[i]='\0';
}

void changeDirectory(char *dirName){
    if(chdir(dirName)==-1){
        printf("Shell>>Error al cambiar de directorio\n");
    }
}

void printWorkingDirectory(){
    char *dirActual=getcwd(NULL,0);
    printf("Shell>>%s\n",dirActual);
    free(dirActual);
}
