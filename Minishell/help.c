#include <stdio.h>
#include <stdlib.h>

/*
    a) Mostrar una ayuda con los comandos posibles.
    b) Crear un directorio.
    c) Eliminar un directorio.
    d) Crear un archivo.
    e) Listar el contenido de un directorio.
    f ) Mostrar el conenido de un archivo.
    g) Modificar los permisos de un archivo. Los permisos son de lectura, escritura y
    ejecución
 */



int main(){
    printf("ls Lista los archivos del directorio actual\n");
    printf("mkdir <Nombre_Directorio> Crea un directorio con un nombre\n");
    printf("rmdir <Nombre_Directorio> Elimina un directorio elegido (Tiene que estar vacio)\n");
    printf("touch <Nombre_Archivo> Crea un archivo\n");
    printf("cat <Nombre_Archivo> Muestra el contenido de un archivo\n");
    printf("chmod <Nombre_Archivo> <Permisos> Modifica los permisos de un archivo\n");
    printf("pwd Muestra el directorio actual\n");
    printf("cd <Nombre_Directorio> Cambia el directorio actual(.. para volver al anterior)\n");
    printf("exit Termina el programa\n");
    return 0;
}