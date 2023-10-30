#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

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
    int type; // 1: Politico, 2: Empresa, 3: Cliente común.
} Cliente;

//Defino los punteros a structs
punteroMensaje fila_entrada;
punteroMensaje fila_politicos;
punteroMensaje fila_empresas;
punteroMensaje fila_comunes;

punteroMensaje cantPoliticosEsperando;
punteroMensaje cantEmpresasEsperando;
punteroMensaje cantComunesEsperando;

punteroMensaje despertarEmpleadoEmpresa;
punteroMensaje despertarEmpleadoComun;
punteroMensaje empleadoEmpresaDormido;
punteroMensaje empleadoComunDormido;

punteroMensaje mutexEmpresa;
punteroMensaje mutexPolitico;
punteroMensaje mutexComun;

punteroMensaje pasarALaOficinaPolitico;
punteroMensaje pasarALaOficinaEmpresa;
punteroMensaje pasarALaOficinaComun;

punteroMensaje atendido;


void *empleadoEmpresa( int fila_empleado) {
    key_t key;
    key = ftok("/tmp", 'A');
    int queueID = msgget(key, 0666 );
    while (1) {

        msgrcv(queueID, mutexPolitico , sizeof(Mensaje) - sizeof(long), tipo_mutexPolitico, 0);
        if (msgrcv(queueID,cantPoliticosEsperando,sizeof (Mensaje)-sizeof (long),tipo_CantPoliticosEsperando,IPC_NOWAIT) != -1) {
            msgsnd(queueID,pasarALaOficinaPolitico,sizeof (Mensaje)-sizeof (long),0);
            msgsnd(queueID,mutexPolitico,sizeof (Mensaje)-sizeof (long),0);
            msgsnd(queueID,fila_empresas,sizeof (Mensaje)-sizeof (long),0);
            usleep(1000);
            printf("Politico es atendido por el empleado %d\n", (fila_empleado + 1));
            sleep(1);
            printf("Politico terminó de ser atendido por el empleado %d\n", (fila_empleado + 1));
            msgsnd(queueID,atendido,sizeof (Mensaje)-sizeof (long),0);
        } else {
            msgsnd(queueID,mutexPolitico,sizeof (Mensaje)-sizeof (long),0);
            msgrcv(queueID,mutexEmpresa,sizeof (Mensaje)-sizeof (long),tipo_mutexEmpresa,0);
            if (msgrcv(queueID,cantEmpresasEsperando,sizeof (Mensaje)-sizeof (long),tipo_CantEmpresasEsperando,IPC_NOWAIT) != -1) {
                msgsnd(queueID,pasarALaOficinaEmpresa,sizeof (Mensaje)-sizeof (long),0);
                msgsnd(queueID,mutexEmpresa,sizeof (Mensaje)-sizeof (long),0);
                msgsnd(queueID,fila_empresas,sizeof (Mensaje)-sizeof (long),0);
                usleep(1000);
                printf("Cliente empresario es atendido por el empleado %d\n", (fila_empleado + 1));
                sleep(1);
                printf("Cliente empresario terminó de ser atendido por el empleado %d\n", (fila_empleado + 1));
                msgsnd(queueID,atendido,sizeof (Mensaje)-sizeof (long),0);
            }
        }

        msgrcv(queueID,mutexEmpresa,sizeof (Mensaje)-sizeof (long),tipo_mutexEmpresa,0);
        if (msgrcv(queueID,cantEmpresasEsperando,sizeof (Mensaje)-sizeof(long),tipo_CantEmpresasEsperando,IPC_NOWAIT) == -1) { // Si no hay clientes de tipo empresa o de tipo políticos me duermo.

            msgsnd(queueID,mutexEmpresa,sizeof (Mensaje)-sizeof (long),0);
            msgrcv(queueID,mutexPolitico,sizeof (Mensaje)-sizeof (long),tipo_mutexPolitico,0);

            if (msgrcv(queueID,cantPoliticosEsperando,sizeof (Mensaje) - sizeof(long),tipo_CantPoliticosEsperando,IPC_NOWAIT ) == -1) {

                msgsnd(queueID,mutexPolitico,sizeof (Mensaje)-sizeof (long),0);
                msgsnd(queueID,empleadoEmpresaDormido,sizeof (Mensaje)-sizeof (long),0);
                printf("El empleado de empresa %d se duerme\n", fila_empleado + 1);
                msgrcv(queueID,despertarEmpleadoEmpresa,sizeof (Mensaje)-sizeof (long),tipo_despertarEmpleadoEmpresa,0);

            } else {

                msgsnd(queueID,cantPoliticosEsperando,sizeof (Mensaje)-sizeof (long),0);
                msgsnd(queueID,mutexPolitico,sizeof (Mensaje)-sizeof (long),0);

            }
        } else {
            msgsnd(queueID,cantEmpresasEsperando,sizeof (Mensaje)-sizeof (long),0);
            msgsnd(queueID,mutexEmpresa,sizeof (Mensaje)-sizeof (long),0);
        }
    }
}

void *empleadoComun( int fila_empleado) {
    key_t key;
    key = ftok("/tmp", 'A');
    int queueID = msgget(key, 0666 );
    while (1) {
        msgrcv(queueID, mutexPolitico , sizeof(Mensaje) - sizeof(long), tipo_mutexPolitico, 0);
        if (msgrcv(queueID,cantPoliticosEsperando,sizeof (Mensaje)-sizeof (long),tipo_CantPoliticosEsperando,IPC_NOWAIT) != -1) {

            msgsnd(queueID,pasarALaOficinaPolitico,sizeof (Mensaje)-sizeof (long),0);
            msgsnd(queueID,mutexPolitico,sizeof (Mensaje)-sizeof (long),0);
            msgsnd(queueID,fila_politicos,sizeof (Mensaje)-sizeof (long),0);
            usleep(1000);
            printf("Politico es atendido por el empleado %d\n", (fila_empleado + 1));
            sleep(1);
            printf("Politico terminó de ser atendido por el empleado %d\n", (fila_empleado + 1));
            msgsnd(queueID,atendido,sizeof (Mensaje)-sizeof (long),0);

        } else {
            msgsnd(queueID,mutexPolitico,sizeof (Mensaje)-sizeof (long),0);
            if (msgrcv(queueID,cantComunesEsperando,sizeof (Mensaje)-sizeof(long),tipo_CantComunesEsperando,IPC_NOWAIT) != -1) {
                msgsnd(queueID,pasarALaOficinaComun,sizeof (Mensaje)-sizeof (long),0);
                msgsnd(queueID,fila_comunes,sizeof (Mensaje)-sizeof (long),0);
                usleep(1000);
                printf("Cliente común es atendido por el empleado %d\n", (fila_empleado + 1));
                sleep(1);
                printf("Cliente común terminó de ser atendido por el empleado %d\n", (fila_empleado + 1));
                msgsnd(queueID,atendido,sizeof (Mensaje)-sizeof (long),0);
            }
        }


        msgrcv(queueID,mutexComun,sizeof (Mensaje)-sizeof (long),tipo_mutexComun,0);

        if (msgrcv(queueID,cantComunesEsperando,sizeof (Mensaje) - sizeof(long),tipo_CantComunesEsperando,IPC_NOWAIT)== -1) { // Si no hay clientes de tipo empresa o de tipo políticos me duermo.
            msgsnd(queueID,mutexComun,sizeof (Mensaje)-sizeof (long),0);
            msgrcv(queueID,mutexPolitico,sizeof (Mensaje)-sizeof (long),tipo_mutexPolitico,0);
            if (msgrcv(queueID,cantPoliticosEsperando,sizeof (Mensaje)-sizeof (long), tipo_CantPoliticosEsperando,0) == -1) {
                msgsnd(queueID,empleadoComunDormido,sizeof (Mensaje)-sizeof (long),0);
                msgsnd(queueID,mutexPolitico,sizeof (Mensaje)-sizeof (long),0);
                printf("El empleado común %d se duerme\n", fila_empleado + 1);
                msgrcv(queueID,despertarEmpleadoComun,sizeof (Mensaje)-sizeof (long),tipo_despertarEmpleadoComun,0);
            } else {
                msgsnd(queueID,cantPoliticosEsperando,sizeof (Mensaje)-sizeof (long),0);
                msgsnd(queueID,mutexPolitico,sizeof (Mensaje)-sizeof (long),0);
            }
        } else {
            msgsnd(queueID,cantComunesEsperando,sizeof (Mensaje)-sizeof (long),0);
            msgsnd(queueID,mutexComun,sizeof (Mensaje)-sizeof (long),0);
        }
    }
}

void *clientePolitico(void *arg) {
    Cliente *cliente = (Cliente *)arg;
    key_t key;
    key = ftok("/tmp", 'A');
    int queueID = msgget(key, 0666 );
    if (msgrcv(queueID,fila_entrada,sizeof (Mensaje)-sizeof (long),tipo_fila_entrada,IPC_NOWAIT) == 0) {
        printf("Politico %d esperando en fila de entrada\n", cliente->id);

        msgrcv(queueID,fila_politicos,sizeof (Mensaje)-sizeof (long),tipo_fila_Politicos,0);
        msgsnd(queueID,cantPoliticosEsperando,sizeof (Mensaje)-sizeof (long),0);

        printf("Politico %d entra a la fila de políticos\n", cliente->id);

        msgrcv(queueID,mutexEmpresa,sizeof (Mensaje)-sizeof (long),tipo_mutexPolitico,0);
        msgrcv(queueID,mutexPolitico,sizeof (Mensaje)-sizeof (long),tipo_mutexEmpresa,0);

        if (msgrcv(queueID, empleadoEmpresaDormido,sizeof (Mensaje)-sizeof(long) , tipo_empleadoEmpresaDormido,IPC_NOWAIT) != -1) {
            msgsnd(queueID,despertarEmpleadoEmpresa,sizeof (Mensaje)-sizeof (long),0);
        } else {
            if (msgrcv(queueID, empleadoComunDormido,sizeof (Mensaje)-sizeof(long) , tipo_empleadoComunDormido,IPC_NOWAIT) != -1) {
                msgsnd(queueID,despertarEmpleadoComun,sizeof (Mensaje)-sizeof (long),0);
            }
        }

        msgsnd(queueID,mutexPolitico,sizeof (Mensaje)-sizeof (long),0);
        msgsnd(queueID,mutexEmpresa,sizeof (Mensaje)-sizeof (long),0);

        msgrcv(queueID,pasarALaOficinaPolitico,sizeof (Mensaje)-sizeof (long),tipo_pasarALaOficinaPolitico,0);
        printf("Politico %d pasa a la oficina\n", cliente->id);
        msgrcv(queueID,atendido,sizeof (Mensaje)-sizeof (long),tipo_atendido,0);
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
    if (msgrcv(queueID,fila_entrada,sizeof (Mensaje)-sizeof (long),tipo_fila_entrada,IPC_NOWAIT) != -1) {
        printf("Empresario %d esperando en fila de entrada\n", cliente->id);

        msgrcv(queueID,fila_empresas,sizeof (Mensaje)-sizeof (long),tipo_fila_Empresa,0);
        msgsnd(queueID,cantEmpresasEsperando,sizeof (Mensaje)-sizeof (long),0);
        printf("Empresario %d entra a la fila de empresas\n", cliente->id);
        msgsnd(queueID,fila_entrada,sizeof (Mensaje)-sizeof (long),0);


        msgrcv(queueID,mutexPolitico,sizeof (Mensaje)-sizeof (long),tipo_mutexPolitico,0);
        msgrcv(queueID,mutexEmpresa,sizeof (Mensaje)-sizeof (long),tipo_mutexEmpresa,0);
        if (msgrcv(queueID, empleadoEmpresaDormido,sizeof (Mensaje)-sizeof (long),tipo_empleadoEmpresaDormido,IPC_NOWAIT) != -1) {
            msgsnd(queueID,despertarEmpleadoEmpresa,sizeof (Mensaje)-sizeof (long),0);
        }

        msgsnd(queueID,mutexEmpresa,sizeof (Mensaje)-sizeof (long),0);
        msgsnd(queueID,mutexPolitico,sizeof (Mensaje)-sizeof (long),0);

        msgrcv(queueID,pasarALaOficinaEmpresa,sizeof (Mensaje)-sizeof (long),tipo_pasarALaOficinaEmpresario,0);
        printf("Empresario %d pasa a la oficina\n", cliente->id);
        msgrcv(queueID,atendido,sizeof (Mensaje)-sizeof (long),tipo_atendido,0);
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
    if (msgrcv(queueID,fila_entrada,sizeof (Mensaje)-sizeof (long),tipo_fila_entrada,IPC_NOWAIT)!= -1) {
        printf("Cliente número %d esperando en fila de entrada\n", cliente->id);

        msgrcv(queueID,fila_comunes,sizeof (Mensaje)-sizeof (long),tipo_fila_ClienteComun,0);
        msgsnd(queueID,cantComunesEsperando,sizeof (Mensaje)-sizeof (long),0);

        printf("Cliente número %d entra a la fila de clientes comunes\n", cliente->id);

        msgsnd(queueID,fila_entrada,sizeof (Mensaje)-sizeof (long),0);

        msgrcv(queueID,mutexComun,sizeof (Mensaje)-sizeof (long),tipo_mutexComun,0);
        msgrcv(queueID,mutexPolitico,sizeof (Mensaje)-sizeof (long),tipo_mutexPolitico,0);

        if(msgrcv(queueID,empleadoComunDormido,sizeof (Mensaje)-sizeof (long),tipo_empleadoComunDormido,IPC_NOWAIT) != -1){
            msgsnd(queueID,despertarEmpleadoComun,sizeof (Mensaje)-sizeof (long),0);
        }

        msgsnd(queueID,mutexPolitico,sizeof (Mensaje)-sizeof (long),0);
        msgsnd(queueID,mutexComun,sizeof (Mensaje)-sizeof (long),0);

        msgrcv(queueID,pasarALaOficinaComun,sizeof (Mensaje)-sizeof (long),tipo_pasarALaOficinaComun,0);
        printf("Cliente número %d pasa a la oficina\n", cliente->id);
        msgrcv(queueID,atendido,sizeof (Mensaje)-sizeof (long),tipo_atendido,0);
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

    //Inicio los structs de los mensajes
    fila_entrada= malloc(sizeof(Mensaje));
    fila_politicos= malloc(sizeof(Mensaje));
    fila_empresas= malloc(sizeof(Mensaje));
    fila_comunes= malloc(sizeof(Mensaje));

    cantPoliticosEsperando= malloc(sizeof(Mensaje));
    cantEmpresasEsperando= malloc(sizeof(Mensaje));
    cantComunesEsperando= malloc(sizeof(Mensaje));

    despertarEmpleadoEmpresa= malloc(sizeof(Mensaje));
    despertarEmpleadoComun= malloc(sizeof(Mensaje));
    empleadoEmpresaDormido= malloc(sizeof(Mensaje));
    empleadoComunDormido= malloc(sizeof(Mensaje));

    mutexEmpresa= malloc(sizeof(Mensaje));
    mutexPolitico= malloc(sizeof(Mensaje));
    mutexComun= malloc(sizeof(Mensaje));

    pasarALaOficinaPolitico= malloc(sizeof(Mensaje));
    pasarALaOficinaEmpresa= malloc(sizeof(Mensaje));
    pasarALaOficinaComun= malloc(sizeof(Mensaje));

    atendido= malloc(sizeof(Mensaje));


    //Inicializo los structs

    fila_entrada->tipo=tipo_fila_entrada;
    //strcpy(fila_entrada->dato1, "\0");

    fila_politicos->tipo=tipo_fila_Politicos;
    //strcpy(fila_politicos->dato1, "\0");

    fila_empresas->tipo=tipo_fila_Empresa;
    //strcpy(fila_empresas->dato1, "\0");

    fila_comunes->tipo=tipo_fila_ClienteComun;
    //strcpy(fila_comunes->dato1, "\0");

    cantPoliticosEsperando->tipo=tipo_CantPoliticosEsperando;
    //strcpy(cantPoliticosEsperando->dato1, "\0");

    cantEmpresasEsperando->tipo=tipo_CantEmpresasEsperando;
    //strcpy(cantEmpresasEsperando->dato1, "\0");

    cantComunesEsperando->tipo=tipo_CantComunesEsperando;
    //strcpy(cantComunesEsperando->dato1, "\0");

    despertarEmpleadoEmpresa->tipo=tipo_despertarEmpleadoEmpresa;
    //strcpy(despertarEmpleadoEmpresa->dato1, "\0");

    despertarEmpleadoComun->tipo=tipo_despertarEmpleadoComun;
    //strcpy(despertarEmpleadoComun->dato1, "\0");

    empleadoEmpresaDormido->tipo=tipo_empleadoEmpresaDormido;
    //strcpy(empleadoEmpresaDormido->dato1, "\0");

    empleadoComunDormido->tipo=tipo_empleadoComunDormido;
    //strcpy(empleadoComunDormido->dato1, "\0");

    mutexEmpresa->tipo=tipo_mutexEmpresa;
    //strcpy(mutexEmpresa->dato1, "\0");

    mutexPolitico->tipo=tipo_mutexPolitico;
    //strcpy(mutexPolitico->dato1, "\0");

    mutexComun->tipo=tipo_mutexComun;
    //strcpy(mutexComun->dato1, "\0");

    pasarALaOficinaPolitico->tipo=tipo_pasarALaOficinaPolitico;
    //strcpy(pasarALaOficinaPolitico->dato1, "\0");

    pasarALaOficinaEmpresa->tipo=tipo_pasarALaOficinaEmpresario;
    //strcpy(pasarALaOficinaEmpresa->dato1, "\0");

    pasarALaOficinaComun->tipo=tipo_pasarALaOficinaComun;
    //strcpy(pasarALaOficinaComun->dato1, "\0");

    atendido->tipo=tipo_atendido;
    //strcpy(atendido->dato1, "\0");










    //Inicializacion de la cola
    for (int i=0;i<MAX_CLIENTES_ENTRADA;i++){
        msgsnd(queueID, fila_entrada, sizeof(Mensaje) - sizeof(long), 0);
    }
    for (int i=0;i<MAX_CLIENTES_FILA;i++){
        msgsnd(queueID, fila_politicos, sizeof(Mensaje) - sizeof(long), 0);
        msgsnd(queueID, fila_empresas, sizeof(Mensaje) - sizeof(long), 0);
        msgsnd(queueID, fila_comunes, sizeof(Mensaje) - sizeof(long), 0);
    }
    msgsnd(queueID, mutexPolitico, sizeof(Mensaje) - sizeof(long), 0);
    msgsnd(queueID, mutexEmpresa, sizeof(Mensaje) - sizeof(long), 0);
    msgsnd(queueID, mutexComun, sizeof(Mensaje) - sizeof(long), 0);


    int pid;

    // Creación de procesos
    int fila[CANT_EMPLEADOS]; // Declarar el array como un arreglo de enteros

    for (int i = 0; i < CANT_EMPLEADOS; i++) {
        fila[i] = i ; // Asignar un valor único a cada empleado
        pid = fork();
        if (pid == 0) {
            if (i == 0) {
                empleadoComun(fila[i]);
            } else {
                empleadoEmpresa(fila[i]);
            }
            exit(0);
        }
    }

    for (int i = 0; i < CANT_PROCESOS; i++) {
        Cliente *cliente = (Cliente *)malloc(sizeof(Cliente));
        cliente->id = i;
        cliente->type = rand() % 3 + 1;
        pid = fork();
        if (pid == 0) {
            switch (cliente->type) {
                case 1: // Politico
                    clientePolitico(cliente);
                    break;
                case 2: // Empresa
                    clienteEmpresa(cliente);
                    break;
                case 3: // Comun
                    clienteComun(cliente);
                    break;
            }
            exit(0);
        }
    }

    for (int i = 0; i < CANT_PROCESOS; i++) {
        wait(NULL);
    }

    if (msgctl(queueID, IPC_RMID, NULL) == -1) {
        perror("Error al eliminar la cola de mensajes");
        exit(1);
    }
    printf("Cierra el banco\n");


    free(fila_entrada);
    free(fila_politicos);
    free(fila_empresas);
    free(fila_comunes);

    free(cantPoliticosEsperando);
    free(cantComunesEsperando);
    free(cantEmpresasEsperando);

    free(despertarEmpleadoEmpresa);
    free(despertarEmpleadoComun);
    free(empleadoEmpresaDormido);
    free(empleadoComunDormido);

    free(mutexEmpresa);
    free(mutexPolitico);
    free(mutexComun);

    free(pasarALaOficinaPolitico);
    free(pasarALaOficinaEmpresa);
    free(pasarALaOficinaComun);

    free(atendido);

    return 0;
}
