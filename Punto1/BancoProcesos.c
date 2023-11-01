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
punteroMensaje mensaje;


void *empleadoEmpresa( int fila_empleado) {
    key_t key;
    key = ftok("/tmp", 'A');
    int queueID = msgget(key, 0666 );
    while (1) {
        mensaje->tipo=tipo_mutexPolitico;
        msgrcv(queueID, mensaje , sizeof(Mensaje) - sizeof(long), tipo_mutexPolitico, 0);
        mensaje->tipo=tipo_CantPoliticosEsperando;
        if (msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_CantPoliticosEsperando,IPC_NOWAIT) != -1) {
            mensaje->tipo=tipo_pasarALaOficinaPolitico;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
            mensaje->tipo=tipo_mutexPolitico;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
            mensaje->tipo=tipo_fila_Empresa;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
            usleep(1000);
            printf("Politico es atendido por el empleado %d\n", (fila_empleado + 1));
            sleep(1);
            printf("Politico terminó de ser atendido por el empleado %d\n", (fila_empleado + 1));
            mensaje->tipo=tipo_atendido;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
        } else {
            mensaje->tipo=tipo_mutexPolitico;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
            mensaje->tipo=tipo_mutexEmpresa;
            msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_mutexEmpresa,0);
            mensaje->tipo=tipo_CantEmpresasEsperando;
            if (msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_CantEmpresasEsperando,IPC_NOWAIT) != -1) {
                mensaje->tipo=tipo_pasarALaOficinaEmpresario;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
                mensaje->tipo=tipo_mutexEmpresa;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
                mensaje->tipo=tipo_fila_Empresa;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
                usleep(1000);
                printf("Cliente empresario es atendido por el empleado %d\n", (fila_empleado + 1));
                sleep(1);
                printf("Cliente empresario terminó de ser atendido por el empleado %d\n", (fila_empleado + 1));
                mensaje->tipo=tipo_atendido;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
            }
        }

        mensaje->tipo=tipo_mutexEmpresa;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_mutexEmpresa,0);
        mensaje->tipo=tipo_CantEmpresasEsperando;
        if (msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof(long),tipo_CantEmpresasEsperando,IPC_NOWAIT) == -1) { // Si no hay clientes de tipo empresa o de tipo políticos me duermo.

            mensaje->tipo=tipo_mutexEmpresa;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
            mensaje->tipo=tipo_mutexPolitico;
            msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_mutexPolitico,0);

            mensaje->tipo=tipo_CantPoliticosEsperando;
            if (msgrcv(queueID,mensaje,sizeof (Mensaje) - sizeof(long),tipo_CantPoliticosEsperando,IPC_NOWAIT ) == -1) {

                mensaje->tipo=tipo_mutexPolitico;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
                mensaje->tipo=tipo_empleadoEmpresaDormido;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
                printf("El empleado de empresa %d se duerme\n", fila_empleado + 1);
                mensaje->tipo=tipo_despertarEmpleadoEmpresa;
                msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_despertarEmpleadoEmpresa,0);

            } else {
                mensaje->tipo=tipo_CantPoliticosEsperando;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
                mensaje->tipo=tipo_mutexPolitico;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);

            }
        } else {
            mensaje->tipo=tipo_CantEmpresasEsperando;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
            mensaje->tipo=tipo_mutexEmpresa;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
        }
    }
}

void *empleadoComun( int fila_empleado) {
    key_t key;
    key = ftok("/tmp", 'A');
    int queueID = msgget(key, 0666 );
    while (1) {
        mensaje->tipo=tipo_mutexPolitico;
        msgrcv(queueID, mensaje , sizeof(Mensaje) - sizeof(long), tipo_mutexPolitico, 0);
        mensaje->tipo=tipo_CantPoliticosEsperando;
        if (msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_CantPoliticosEsperando,IPC_NOWAIT) != -1) {

            mensaje->tipo=tipo_pasarALaOficinaPolitico;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
            mensaje->tipo=tipo_mutexPolitico;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
            mensaje->tipo=tipo_fila_Politicos;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
            usleep(1000);
            printf("Politico es atendido por el empleado %d\n", (fila_empleado + 1));
            sleep(1);
            printf("Politico terminó de ser atendido por el empleado %d\n", (fila_empleado + 1));
            mensaje->tipo=tipo_atendido;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);

        } else {
            mensaje->tipo=tipo_mutexPolitico;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
            mensaje->tipo=tipo_CantComunesEsperando;
            if (msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof(long),tipo_CantComunesEsperando,IPC_NOWAIT) != -1) {
                mensaje->tipo=tipo_pasarALaOficinaComun;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
                mensaje->tipo=tipo_fila_ClienteComun;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
                usleep(1000);
                printf("Cliente común es atendido por el empleado %d\n", (fila_empleado + 1));
                sleep(1);
                printf("Cliente común terminó de ser atendido por el empleado %d\n", (fila_empleado + 1));
                mensaje->tipo=tipo_atendido;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
            }
        }

        mensaje->tipo=tipo_mutexComun;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_mutexComun,0);

        mensaje->tipo=tipo_CantComunesEsperando;
        if (msgrcv(queueID,mensaje,sizeof (Mensaje) - sizeof(long),tipo_CantComunesEsperando,IPC_NOWAIT)== -1) { // Si no hay clientes de tipo empresa o de tipo políticos me duermo.
            mensaje->tipo=tipo_mutexComun;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
            mensaje->tipo=tipo_mutexPolitico;
            msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_mutexPolitico,0);
            mensaje->tipo=tipo_CantPoliticosEsperando;
            if (msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long), tipo_CantPoliticosEsperando,0) == -1) {
                mensaje->tipo=tipo_empleadoComunDormido;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
                mensaje->tipo=tipo_mutexPolitico;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
                printf("El empleado común %d se duerme\n", fila_empleado + 1);
                mensaje->tipo=tipo_despertarEmpleadoComun;
                msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_despertarEmpleadoComun,0);
            } else {
                mensaje->tipo=tipo_CantPoliticosEsperando;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
                mensaje->tipo=tipo_mutexPolitico;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
            }
        } else {
            mensaje->tipo=tipo_CantComunesEsperando;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
            mensaje->tipo=tipo_mutexComun;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
        }
    }
}

void *clientePolitico(void *arg) {
    Cliente *cliente = (Cliente *)arg;
    key_t key;
    key = ftok("/tmp", 'A');
    int queueID = msgget(key, 0666 );
    mensaje->tipo=tipo_fila_entrada;
    if (msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_fila_entrada,IPC_NOWAIT) == 0) {
        printf("Politico %d esperando en fila de entrada\n", cliente->id);

        mensaje->tipo=tipo_fila_Politicos;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_fila_Politicos,0);
        mensaje->tipo=tipo_CantPoliticosEsperando;
        msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);

        printf("Politico %d entra a la fila de políticos\n", cliente->id);
        mensaje->tipo=tipo_mutexPolitico;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_mutexPolitico,0);
        mensaje->tipo=tipo_mutexEmpresa;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_mutexEmpresa,0);

        mensaje->tipo=tipo_empleadoEmpresaDormido;
        if (msgrcv(queueID, mensaje,sizeof (Mensaje)-sizeof(long) , tipo_empleadoEmpresaDormido,IPC_NOWAIT) != -1) {
            mensaje->tipo=tipo_despertarEmpleadoEmpresa;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
        } else {
            mensaje->tipo=tipo_empleadoComunDormido;
            if (msgrcv(queueID, mensaje,sizeof (Mensaje)-sizeof(long) , tipo_empleadoComunDormido,IPC_NOWAIT) != -1) {
                mensaje->tipo=tipo_despertarEmpleadoComun;
                msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
            }
        }

        mensaje->tipo=tipo_mutexPolitico;
        msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
        mensaje->tipo=tipo_mutexEmpresa;
        msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);

        mensaje->tipo=tipo_pasarALaOficinaPolitico;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_pasarALaOficinaPolitico,0);
        printf("Politico %d pasa a la oficina\n", cliente->id);
        mensaje->tipo=tipo_atendido;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_atendido,0);
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
    if (msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_fila_entrada,IPC_NOWAIT) != -1) {
        printf("Empresario %d esperando en fila de entrada\n", cliente->id);

        mensaje->tipo=tipo_fila_Empresa;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_fila_Empresa,0);
        mensaje->tipo=tipo_CantEmpresasEsperando;
        msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
        printf("Empresario %d entra a la fila de empresas\n", cliente->id);
        mensaje->tipo=tipo_fila_entrada;
        msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);

        mensaje->tipo=tipo_mutexPolitico;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_mutexPolitico,0);
        mensaje->tipo=tipo_mutexEmpresa;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_mutexEmpresa,0);
        mensaje->tipo=tipo_empleadoEmpresaDormido;
        if (msgrcv(queueID, mensaje,sizeof (Mensaje)-sizeof (long),tipo_empleadoEmpresaDormido,IPC_NOWAIT) != -1) {
            mensaje->tipo=tipo_despertarEmpleadoEmpresa;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
        }

        mensaje->tipo=tipo_mutexEmpresa;
        msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
        mensaje->tipo=tipo_mutexPolitico;
        msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);

        mensaje->tipo=tipo_pasarALaOficinaEmpresario;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_pasarALaOficinaEmpresario,0);
        printf("Empresario %d pasa a la oficina\n", cliente->id);
        mensaje->tipo=tipo_atendido;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_atendido,0);
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
    if (msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_fila_entrada,IPC_NOWAIT)!= -1) {
        printf("Cliente número %d esperando en fila de entrada\n", cliente->id);

        mensaje->tipo=tipo_fila_ClienteComun;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_fila_ClienteComun,0);
        mensaje->tipo=tipo_CantComunesEsperando;
        msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);

        printf("Cliente número %d entra a la fila de clientes comunes\n", cliente->id);

        mensaje->tipo=tipo_fila_entrada;
        msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);

        mensaje->tipo=tipo_mutexComun;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_mutexComun,0);
        mensaje->tipo=tipo_mutexPolitico;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_mutexPolitico,0);

        mensaje->tipo=tipo_empleadoComunDormido;
        if(msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_empleadoComunDormido,IPC_NOWAIT) != -1){
            mensaje->tipo=tipo_despertarEmpleadoComun;
            msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
        }

        mensaje->tipo=tipo_mutexPolitico;
        msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);
        mensaje->tipo=tipo_mutexComun;
        msgsnd(queueID,mensaje,sizeof (Mensaje)-sizeof (long),0);

        mensaje->tipo=tipo_pasarALaOficinaComun;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_pasarALaOficinaComun,0);
        printf("Cliente número %d pasa a la oficina\n", cliente->id);
        mensaje->tipo=tipo_atendido;
        msgrcv(queueID,mensaje,sizeof (Mensaje)-sizeof (long),tipo_atendido,0);
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
        queueID = msgget(key, 0666 | IPC_CREAT);
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

    for (int i = 0; i < CANT_EMPLEADOS; ++i) {
        kill(pidEmpleados[i],SIGTERM);
    }


    if (msgctl(queueID, IPC_RMID, NULL) == -1) {
        perror("Error al eliminar la cola de mensajes");
        exit(1);
    }
    printf("Cierra el banco\n");


    free(mensaje);
    free(pidEmpleados);


    return 0;
}
