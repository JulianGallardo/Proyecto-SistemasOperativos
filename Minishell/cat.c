#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]){
    FILE *fp;
    char ch;
    if(argc==1){
        printf("Shell>>No se ingreso el nombre del archivo\n");
    }
    else{
        if((fp=fopen(argv[1],"r"))!=NULL){
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