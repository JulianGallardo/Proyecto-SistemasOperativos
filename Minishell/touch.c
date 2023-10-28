#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    FILE *fp;
    char ch;
    if(argc==0){
        printf("Shell>>No se ingreso el nombre del archivo\n");
    }
    else{
        if((fp=fopen(argv[1],"w+"))!=NULL){//Se abre el archivo en modo lectura
            while((ch=fgetc(fp))!=EOF){
                printf("%c",ch);
            }
            fclose(fp);
        }
        else{
            perror("");
            return EXIT_FAILURE;
        }
    }

}