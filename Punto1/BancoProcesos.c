#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <time.h>

#define MAX_CLIENTES_ENTRADA 30
#define MAX_CLIENTES_FILA 15
#define CANT_PROCESOS 90
#define CANT_EMPLEADOS 3

#define tipo_fila_entrada 1
#define tipo_fila_Politicos 2
#define tipo_fila_Empresa 3
#define tipo_fila_ClienteComun 4

#define tipo_CantPoliticosEsperando 5
#define tipo_CantEmpresasEsperando 6
#define tipo_CantComunesEsperando 7

#define tipo_despertarEmpleadoEmpresa 8
#define tipo_despertarEmpleadoComun 9
#define tipo_empleadoEmpresaDormido 10
#define tipo_empleadoComunDormido 11

#define tipo_mutexEmpresa 12
#define tipo_mutexPolitico 13
#define tipo_mutexComun 14

#define tipo_pasarALaOficinaEmpresario 15
#define tipo_pasarALaOficinaPolitico 16
#define tipo_pasarALaOficinaComun 17

#define tipo_atendido 18

typedef struct{
    long tipo;
}Mensaje;

typedef Mensaje* punteroMensaje;

typedef struct {
    int id;
    int type;
} Cliente;

//Defino los punteros a structs
punteroMensaje mensaje;


int numeroRandom(int adicion){
    // Semilla para la generación de números aleatorios
    srand(time(NULL)+adicion);
    int result =(int)rand();
    return result;
}

void *empleadoEmpresa( int fila_empleado) {
    key_t key;
    key = ftok("/tmp", 'A');
    int queueID = msgget(key, 0666 );
    while (1) {
        mensaje->tipo=tipo_mutexPolitico;
        msgrcv(queueID, mensaje , sizeof(Mensaje) - sizeof(long), tipo_mutexPolitico, 0);//Pido el mutex de políticos
        mensaje->tipo=tipo_CantPoliticosEsperando;
        if (msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_CantPoliticosEsperando,IPC_NOWAIT) != -1) {//Si hay politicos esperando, atiendo a uno
            mensaje->tipo=tipo_pasarALaOficinaPolitico;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Aviso que lo llamo a la oficina
            mensaje->tipo=tipo_mutexPolitico;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Libero el mutex de políticos
            mensaje->tipo=tipo_fila_Politicos;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Aviso que ya no esta el cliente en la fila de politicos
            usleep(1000);
            printf("Politico es atendido por el empleado %d\n", (fila_empleado + 1));
            sleep(1);
            printf("Politico terminó de ser atendido por el empleado %d\n", (fila_empleado + 1));
            mensaje->tipo=tipo_atendido;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Aviso que termino de atender al cliente
        } else {//Si no hay politicos esperando, consulto si hay clientes de tipo empresa esperando
            mensaje->tipo=tipo_mutexPolitico;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0); //Libero el mutex de políticos
            mensaje->tipo=tipo_mutexEmpresa;
            msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_mutexEmpresa,0);//Pido el mutex de empresas
            mensaje->tipo=tipo_CantEmpresasEsperando;
            if (msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_CantEmpresasEsperando,IPC_NOWAIT) != -1) {//Si hay clientes de tipo empresa esperando, atiendo a uno
                mensaje->tipo=tipo_pasarALaOficinaEmpresario;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Aviso que lo llamo a la oficina
                mensaje->tipo=tipo_mutexEmpresa;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Libero el mutex de empresas
                mensaje->tipo=tipo_fila_Empresa;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Aviso que ya no esta el cliente en la fila de empresas
                usleep(1000);
                printf("Cliente empresario es atendido por el empleado %d\n", (fila_empleado + 1));
                sleep(1);
                printf("Cliente empresario terminó de ser atendido por el empleado %d\n", (fila_empleado + 1));
                mensaje->tipo=tipo_atendido;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Aviso que termino de atender al cliente
            }
        }

        mensaje->tipo=tipo_mutexEmpresa;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_mutexEmpresa,0);//Pido el mutex de empresas
        mensaje->tipo=tipo_CantEmpresasEsperando;
        if (msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof(long),tipo_CantEmpresasEsperando,IPC_NOWAIT) == -1) { // Si no hay clientes de tipo empresa o de tipo políticos me duermo.

            mensaje->tipo=tipo_mutexEmpresa;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Libero el mutex de empresas
            mensaje->tipo=tipo_mutexPolitico;
            msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_mutexPolitico,0);//Pido el mutex de políticos

            mensaje->tipo=tipo_CantPoliticosEsperando;
            if (msgrcv(queueID,mensaje,sizeof (Mensaje) - sizeof(long),tipo_CantPoliticosEsperando,IPC_NOWAIT ) == -1) {//Si no hay clientes de tipo político me duermo

                mensaje->tipo=tipo_mutexPolitico;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Libero el mutex de políticos
                mensaje->tipo=tipo_empleadoEmpresaDormido;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Aviso que me duermo
                printf("El empleado de empresa %d se duerme\n", fila_empleado + 1);
                mensaje->tipo=tipo_despertarEmpleadoEmpresa;
                msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_despertarEmpleadoEmpresa,0);//Espero a que me despierten

            } else {
                mensaje->tipo=tipo_CantPoliticosEsperando;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Devuelvo el politico sacado para chequear, avisando que todavia hay politicos
                mensaje->tipo=tipo_mutexPolitico;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Libero el mutex de políticos

            }
        } else {
            mensaje->tipo=tipo_CantEmpresasEsperando;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Devuelvo el cliente de tipo empresa sacado para chequear, avisando que todavia hay clientes de tipo empresa
            mensaje->tipo=tipo_mutexEmpresa;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Libero el mutex de empresa
        }
    }
}

void *empleadoComun( int fila_empleado) {
    key_t key;
    key = ftok("/tmp", 'A');
    int queueID = msgget(key, 0666 );
    while (1) {
        mensaje->tipo=tipo_mutexPolitico;
        msgrcv(queueID, mensaje , sizeof(Mensaje) - sizeof(long), tipo_mutexPolitico, 0);//Pido el mutex de políticos
        mensaje->tipo=tipo_CantPoliticosEsperando;
        if (msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_CantPoliticosEsperando,IPC_NOWAIT) != -1) {//Si hay politicos esperando, atiendo a uno

            mensaje->tipo=tipo_pasarALaOficinaPolitico;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Aviso que lo llamo a la oficina
            mensaje->tipo=tipo_mutexPolitico;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Libero el mutex de políticos
            mensaje->tipo=tipo_fila_Politicos;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Aviso que ya no esta el cliente en la fila de politicos
            usleep(1000);
            printf("Politico es atendido por el empleado %d\n", (fila_empleado + 1));
            sleep(1);
            printf("Politico terminó de ser atendido por el empleado %d\n", (fila_empleado + 1));
            mensaje->tipo=tipo_atendido;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Aviso que termino de atender al cliente

        } else {
            mensaje->tipo=tipo_mutexPolitico;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Libero el mutex de políticos
            mensaje->tipo=tipo_CantComunesEsperando;
            if (msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof(long),tipo_CantComunesEsperando,IPC_NOWAIT) != -1) {//Si hay clientes de tipo común esperando, atiendo a uno
                mensaje->tipo=tipo_pasarALaOficinaComun;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Aviso que lo llamo a la oficina
                mensaje->tipo=tipo_fila_ClienteComun;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Aviso que ya no esta el cliente en la fila de clientes comunes
                usleep(1000);
                printf("Cliente común es atendido por el empleado %d\n", (fila_empleado + 1));
                sleep(1);
                printf("Cliente común terminó de ser atendido por el empleado %d\n", (fila_empleado + 1));
                mensaje->tipo=tipo_atendido;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Aviso que termino de atender al cliente
            }
        }

        mensaje->tipo=tipo_mutexComun;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_mutexComun,0);

        mensaje->tipo=tipo_CantComunesEsperando;
        if (msgrcv(queueID,mensaje,sizeof (Mensaje) - sizeof(long),tipo_CantComunesEsperando,IPC_NOWAIT)== -1) { // Si no hay clientes de tipo empresa o de tipo políticos me duermo.
            mensaje->tipo=tipo_mutexComun;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0); //Libero el mutex de clientes comunes
            mensaje->tipo=tipo_mutexPolitico;
            msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_mutexPolitico,0); //Pido el mutex de políticos
            mensaje->tipo=tipo_CantPoliticosEsperando;
            if (msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long), tipo_CantPoliticosEsperando,0) == -1) { //Si no hay clientes de tipo político me duermo
                mensaje->tipo=tipo_empleadoComunDormido;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Aviso que me duermo
                mensaje->tipo=tipo_mutexPolitico;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Libero el mutex de políticos
                printf("El empleado común %d se duerme\n", fila_empleado + 1);
                mensaje->tipo=tipo_despertarEmpleadoComun;
                msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_despertarEmpleadoComun,0);//Espero a que me despierten
            } else {
                mensaje->tipo=tipo_CantPoliticosEsperando;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Devuelvo el politico sacado para chequear, avisando que todavia hay politicos
                mensaje->tipo=tipo_mutexPolitico;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Libero el mutex de políticos
            }
        } else {
            mensaje->tipo=tipo_CantComunesEsperando;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Devuelvo el cliente de tipo común sacado para chequear, avisando que todavia hay clientes de tipo común
            mensaje->tipo=tipo_mutexComun;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Libero el mutex de clientes comunes
        }
    }
}

void *clientePolitico(void *arg) {
    Cliente *cliente = (Cliente *)arg;
    key_t key;
    key = ftok("/tmp", 'A');
    int queueID = msgget(key, 0666 );
    mensaje->tipo=tipo_fila_entrada;
    if (msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_fila_entrada,IPC_NOWAIT) == 0) {//Si hay lugar en la fila de entrada
        printf("Politico %d esperando en fila de entrada\n", cliente->id);

        mensaje->tipo=tipo_fila_Politicos;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_fila_Politicos,0);//Espero a que haya lugar en la fila de políticos
        mensaje->tipo=tipo_CantPoliticosEsperando;
        msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Aviso que hay un cliente de tipo político esperando

        printf("Politico %d entra a la fila de políticos\n", cliente->id);
        mensaje->tipo=tipo_mutexPolitico;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_mutexPolitico,0);//Pido el mutex de políticos
        mensaje->tipo=tipo_mutexEmpresa;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_mutexEmpresa,0);//Pido el mutex de empresas

        mensaje->tipo=tipo_empleadoEmpresaDormido;
        if (msgrcv(queueID, mensaje,sizeof (Mensaje)-sizeof(long) , tipo_empleadoEmpresaDormido,IPC_NOWAIT) != -1) {//Si hay un empleado de empresa dormido lo despierto
            mensaje->tipo=tipo_despertarEmpleadoEmpresa;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
        } else {
            mensaje->tipo=tipo_empleadoComunDormido;
            if (msgrcv(queueID, mensaje,sizeof (Mensaje)-sizeof(long) , tipo_empleadoComunDormido,IPC_NOWAIT) != -1) {//Si hay un empleado común dormido lo despierto
                mensaje->tipo=tipo_despertarEmpleadoComun;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
            }
        }

        mensaje->tipo=tipo_mutexPolitico;
        msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Libero el mutex de políticos
        mensaje->tipo=tipo_mutexEmpresa;
        msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Libero el mutex de empresas

        mensaje->tipo=tipo_pasarALaOficinaPolitico;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_pasarALaOficinaPolitico,0);//Espero a que me llamen a la oficina
        printf("Politico %d pasa a la oficina\n", cliente->id);
        mensaje->tipo=tipo_atendido;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_atendido,0);//Espero a que me terminen de atender
        printf("Politico %d se retira\n", cliente->id);
    } else {
        printf("Politico %d se retira ya que no hay lugar en la fila de entrada.\n", cliente->id);
    }

    free(cliente);
    exit(0);
}

void *clienteEmpresa(void *arg) {
    Cliente *cliente = (Cliente *)arg;
    key_t key;
    key = ftok("/tmp", 'A');
    int queueID = msgget(key, 0666 );
    mensaje->tipo=tipo_fila_entrada;
    if (msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_fila_entrada,IPC_NOWAIT) != -1) {//Si hay lugar en la fila de entrada
        printf("Empresario %d esperando en fila de entrada\n", cliente->id);

        mensaje->tipo=tipo_fila_Empresa;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_fila_Empresa,0); // Espero a que haya lugar en la fila de empresas
        mensaje->tipo=tipo_CantEmpresasEsperando;
        msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Aviso que hay un cliente de tipo empresa esperando
        printf("Empresario %d entra a la fila de empresas\n", cliente->id);
        mensaje->tipo=tipo_fila_entrada;
        msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Aviso que ya no estoy en la fila de entrada

        mensaje->tipo=tipo_mutexPolitico;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_mutexPolitico,0);//Pido el mutex de políticos
        mensaje->tipo=tipo_mutexEmpresa;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_mutexEmpresa,0);//Pido el mutex de empresas
        mensaje->tipo=tipo_empleadoEmpresaDormido;
        if (msgrcv(queueID, mensaje,sizeof (Mensaje)-sizeof (long),tipo_empleadoEmpresaDormido,IPC_NOWAIT) != -1) {//Si hay un empleado de empresa dormido lo despierto
            mensaje->tipo=tipo_despertarEmpleadoEmpresa;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
        }

        mensaje->tipo=tipo_mutexEmpresa;
        msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Libero el mutex de empresas
        mensaje->tipo=tipo_mutexPolitico;
        msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Libero el mutex de políticos

        mensaje->tipo=tipo_pasarALaOficinaEmpresario;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_pasarALaOficinaEmpresario,0);//Espero a que me llamen a la oficina
        printf("Empresario %d pasa a la oficina\n", cliente->id);
        mensaje->tipo=tipo_atendido;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_atendido,0);//Espero a que me terminen de atender
        printf("Empresario %d se retira\n", cliente->id);
    } else {
        printf("Empresario %d se retira ya que no hay lugar en la fila de entrada.\n", cliente->id);
    }
    free(cliente);
    exit(0);
}

void *clienteComun(void *arg) {
    Cliente *cliente = (Cliente *)arg;
    key_t key;
    key = ftok("/tmp", 'A');
    int queueID = msgget(key, 0666 );
    mensaje->tipo=tipo_fila_entrada;
    if (msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_fila_entrada,IPC_NOWAIT)!= -1) {//Si hay lugar en la fila de entrada
        printf("Cliente número %d esperando en fila de entrada\n", cliente->id);

        mensaje->tipo=tipo_fila_ClienteComun;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_fila_ClienteComun,0);//Espero a que haya lugar en la fila de clientes comunes
        mensaje->tipo=tipo_CantComunesEsperando;
        msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Aviso que hay un cliente de tipo común esperando

        printf("Cliente número %d entra a la fila de clientes comunes\n", cliente->id);

        mensaje->tipo=tipo_fila_entrada;
        msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Aviso que ya no estoy en la fila de entrada

        mensaje->tipo=tipo_mutexComun;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_mutexComun,0);//Pido el mutex de clientes comunes
        mensaje->tipo=tipo_mutexPolitico;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_mutexPolitico,0);//Pido el mutex de políticos

        mensaje->tipo=tipo_empleadoComunDormido;
        if(msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_empleadoComunDormido,IPC_NOWAIT) != -1){//Si hay un empleado común dormido lo despierto
            mensaje->tipo=tipo_despertarEmpleadoComun;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
        }

        mensaje->tipo=tipo_mutexPolitico;
        msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Libero el mutex de políticos
        mensaje->tipo=tipo_mutexComun;
        msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);//Libero el mutex de clientes comunes

        mensaje->tipo=tipo_pasarALaOficinaComun;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_pasarALaOficinaComun,0);//Espero a que me llamen a la oficina
        printf("Cliente número %d pasa a la oficina\n", cliente->id);
        mensaje->tipo=tipo_atendido;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_atendido,0);//Espero a que me terminen de atender
        printf("Cliente número %d se retira\n", cliente->id);
    } else {
        printf("Cliente número %d se retira ya que no hay lugar en la fila de entrada.\n", cliente->id);
    }
    free(cliente);
    exit(0);
}

int main() {
    // Inicio la cola de mensajes
    key_t key;
    key = ftok("/tmp", 'A');
    int queueID = msgget(key, 0666 | IPC_CREAT);
    if(queueID != -1){ //Si la cola ya existe la elimino y la vuelvo a crear
        msgctl(queueID, IPC_RMID, NULL);
        queueID = msgget(key,  IPC_CREAT);
    }

    //Inicio el struct de mensajes
    mensaje = (punteroMensaje)malloc(sizeof(Mensaje));

    //Inicializacion de la cola
    mensaje->tipo=tipo_fila_entrada;
    for (int i=0;i<MAX_CLIENTES_ENTRADA;i++){
        msgsnd(queueID, mensaje, sizeof(Mensaje) - sizeof(long), 0);
    }
    for (int i=0;i<MAX_CLIENTES_FILA;i++){
        mensaje->tipo=tipo_fila_Politicos;
        msgsnd(queueID, mensaje, sizeof(Mensaje) - sizeof(long), 0);
        mensaje->tipo=tipo_fila_Empresa;
        msgsnd(queueID, mensaje, sizeof(Mensaje) - sizeof(long), 0);
        mensaje->tipo=tipo_fila_ClienteComun;
        msgsnd(queueID, mensaje, sizeof(Mensaje) - sizeof(long), 0);
    }
    mensaje->tipo=tipo_mutexPolitico;
    msgsnd(queueID, mensaje, sizeof(Mensaje) - sizeof(long), 0);
    mensaje->tipo=tipo_mutexEmpresa;
    msgsnd(queueID, mensaje, sizeof(Mensaje) - sizeof(long), 0);
    mensaje->tipo=tipo_mutexComun;
    msgsnd(queueID, mensaje, sizeof(Mensaje) - sizeof(long), 0);



    int pid;
    int* pidEmpleados= malloc(sizeof (int)*CANT_EMPLEADOS);

    // Creación de procesos
    int fila[CANT_EMPLEADOS]; // Declarar el array como un arreglo de enteros

    for (int i = 0; i < CANT_EMPLEADOS; i++) {
        fila[i] = i ; // Asignar un valor único a cada empleado
        pid = fork();
        if (pid == 0) {
            pidEmpleados[i]=getpid();
            if (i == 0) {
                empleadoComun(fila[i]);
            } else {
                empleadoEmpresa(fila[i]);
            }
            exit(0);
        }
    }

    printf("Abre el banco\n");

    for (int i = 0; i < CANT_PROCESOS; i++) {
        Cliente *cliente = (Cliente *)malloc(sizeof(Cliente));
        cliente->id = i;
        cliente->type = numeroRandom(i)%100 + 1;
        pid = fork();
        if (pid == 0) {
           if (cliente->type <= 30) {
                clientePolitico(cliente);
            } else if (cliente->type <= 65) {
                clienteEmpresa(cliente);
            } else {
                clienteComun(cliente);
            }
            exit(0);
        }
    }

    //Esperamos a que terminen todos los clientes
    for (int i = 0; i < CANT_PROCESOS; i++) {
        wait(NULL);
    }

    //Terminamos los procesos de los empleados
    for (int i = 0; i < CANT_EMPLEADOS; ++i) {
        kill(pidEmpleados[i],SIGTERM);
    }

    //Una vez que terminen todos, eliminamos la cola de mensajes
    if (msgctl(queueID, IPC_RMID, NULL) == -1) {
        perror("Error al eliminar la cola de mensajes");
        exit(1);
    }
    printf("Cierra el banco\n");


    free(mensaje);
    free(pidEmpleados);


    return 0;
}
