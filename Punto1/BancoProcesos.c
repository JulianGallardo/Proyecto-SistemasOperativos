#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>


#define MAX_CLIENTES_ENTRADA 30
#define MAX_CLIENTES_FILA 15
#define CANT_PROCESOS 80
#define CANT_EMPLEADOS 3

/**
 * Semáforos
 * - fila_entrada: Controla la cantidad de clientes que pueden entrar al banco. Tipo 1
 * - mutexPolitico: Controla el acceso a la mesa de entrada de los clientes de tipo político. Tipo 2
 * - mutexEmpresa: Controla el acceso a la mesa de entrada de los clientes de tipo empresa. Tipo 3
 * - fila_Politicos: Controla la cantidad de clientes de tipo político que pueden esperar en la fila. Tipo 4
 * - fila_Empresa: Controla la cantidad de clientes de tipo empresa que pueden esperar en la fila. Tipo 5
 * - fila_ClienteComun: Controla la cantidad de clientes de tipo común que pueden esperar en la fila. Tipo 6
 * - CantPoliticosEsperando: Cantidad de clientes de tipo político esperando en la fila. Tipo 7
 * - CantEmpresasEsperando: Cantidad de clientes de tipo empresa esperando en la fila. Tipo 8
 * - CantComunesEsperando: Cantidad de clientes de tipo común esperando en la fila. Tipo 9
 * - pasarALaOficinaPolitico: Controla el acceso a la oficina de los clientes de tipo político. Tipo 10
 * - pasarALaOficinaEmpresario: Controla el acceso a la oficina de los clientes de tipo empresa. Tipo 11
 * - pasarALaOficinaComun: Controla el acceso a la oficina de los clientes de tipo común. Tipo 12
 * - atendido: Controla que el cliente haya sido atendido. Tipo 13
 * - empleadoEmpresaDormido: Controla que el empleado de empresa esté dormido. Tipo 14
 * - empleadoComunDormido: Controla que el empleado común esté dormido. Tipo 15
 * - despertarEmpleadoEmpresa: Controla que el empleado de empresa sea despertado. Tipo 16
 * - despertarEmpleadoComun: Controla que el empleado común sea despertado. Tipo 17
 *
 */


struct mensaje{
    long tipo;
    char dato1[20];
};

typedef struct {
    int id;
    int type; // 1: Politico, 2: Empresa, 3: Cliente común.
} Cliente;


void *empleadoEmpresa(void *arg) {
    int fila_empleado = (int)arg;
    while(1) {

        read(mutexPolitico[0], NULL, 1); // Espera a que haya lugar en la mesa de entrada
        if(sem_trywait(&CantPoliticosEsperando) == 0) {
            sem_post(&pasarALaOficinaPolitico);
            sem_post(&mutexPolitico);
            sem_post(&fila_Politicos);
            usleep(1000);
            printf("Politico es atendido por el empleado ---%d\n", (fila_empleado + 1));
            sleep(1);
            printf("Politico termino de ser atendido por el empleado %d\n", (fila_empleado + 1));
            sem_post(&atendido);
        }
        else {
            sem_post(&mutexPolitico);
            sem_wait(&mutexEmpresa);
            if(sem_trywait(&CantEmpresasEsperando)==0) {
                sem_post(&pasarALaOficinaEmpresario);
                sem_post(&mutexEmpresa);
                sem_post(&fila_Empresa);
                usleep(1000);
                printf("Cliente empresario es atendido por el empleado ---%d\n", (fila_empleado + 1));
                sleep(1);
                printf("Cliente empresario termino de ser atendido por el empleado %d\n", (fila_empleado + 1));
                sem_post(&atendido);
            }
            else{
                sem_post(&mutexEmpresa);
            }
        }

        sem_wait(&mutexEmpresa);
        if(sem_trywait(&CantEmpresasEsperando)==-1){//Si no hay clientes de tipo empresa o de tipo politicos me duermo.
            sem_post(&mutexEmpresa);
            sem_wait(&mutexPolitico);
            if(sem_trywait(&CantPoliticosEsperando)==-1){
                sem_post(&mutexPolitico);
                sem_post(&empleadoEmpresaDormido);
                printf("El empleado de empresa %d se duerme\n",fila_empleado+1);
                sem_wait(&despertarEmpleadoEmpresa);
            }
            else{
                sem_post(&CantPoliticosEsperando);
                sem_post(&mutexPolitico);
            }

        }
        else{
            sem_post(&CantEmpresasEsperando);
            sem_post(&mutexEmpresa);
        }

    }
}

void *empleadoComun(void *arg) {
    int fila_empleado = (int)arg;

    while(1){

        sem_wait(&mutexPolitico);
        if(sem_trywait(&CantPoliticosEsperando) == 0) {
            sem_post(&pasarALaOficinaPolitico);
            sem_post(&mutexPolitico);
            sem_post(&fila_Politicos);
            usleep(1000);
            printf("Politico es atendido por el empleado ---%d\n", (fila_empleado + 1));
            sleep(1);
            printf("Politico termino de ser atendido por el empleado %d\n", (fila_empleado + 1));
            sem_post(&atendido);
        }
        else {
            sem_post(&mutexPolitico);
            if(sem_trywait(&CantComunesEsperando)==0) {
                sem_post(&pasarALaOficinaComun);
                sem_post(&fila_ClienteComun);
                usleep(1000);
                printf("Cliente comun es atendido por el empleado --- %d\n", (fila_empleado + 1));
                sleep(1);
                printf("Cliente comun termino de ser atendido por el empleado %d\n", (fila_empleado + 1));
                sem_post(&atendido);
            }
        }

        if(sem_trywait(&CantComunesEsperando)==-1){//Si no hay clientes de tipo empresa o de tipo politicos me duermo.
            sem_wait(&mutexPolitico);
            if(sem_trywait(&CantPoliticosEsperando)==-1){
                sem_post(&empleadoComunDormido);
                sem_post(&mutexPolitico);
                printf("El empleado comun %d se duerme\n",fila_empleado+1);
                sem_wait(&despertarEmpleadoComun);
            }
            else{
                sem_post(&CantPoliticosEsperando);
                sem_post(&mutexPolitico);
            }
        }
        else{
            sem_post(&CantComunesEsperando);
        }
    }
}

void* clientePolitico(void* arg){
    Cliente *cliente = (Cliente *)arg; //Casteo del argumento a cliente.

    if(sem_trywait(&fila_entrada) == 0) {
        printf("Politico %d esperando en fila de entrada\n", cliente->id);

        sem_wait(&fila_Politicos);
        sem_post(&CantPoliticosEsperando);
        printf("Politico %d entra a la fila de politicos\n",cliente->id);
        sem_post(&fila_entrada);

        if(sem_trywait(&empleadoEmpresaDormido)==0){//Si hay un empleadoEmpresa dormido lo despierta
            sem_post(&despertarEmpleadoEmpresa);
        }
        else{//Si el empleadoEmpresa no esta dormido, intenta despertar al empleado comun.
            if(sem_trywait(&empleadoComunDormido)==0){//Si hay un empleadoComun dormido lo despierta
                sem_post(&despertarEmpleadoComun);
            }
        }
        sem_wait(&pasarALaOficinaPolitico);
        printf("Politico %d pasa a la oficina\n",cliente->id);
        sem_wait(&atendido);
        printf("Politico %d se retira\n",cliente->id);
    }
    else {
        printf("Politico %d se retira ya que no hay lugar en la fila de entrada.\n", cliente->id);
    }

    free(cliente);
    exit(0);
}

void* clienteEmpresa(void* arg){
    Cliente *cliente = (Cliente *)arg; //Casteo del argumento a cliente.

    if(sem_trywait(&fila_entrada) == 0) {
        printf("Empresario %d esperando en fila de entrada\n", cliente->id);
        sem_wait(&fila_Empresa);
        sem_post(&CantEmpresasEsperando);
        printf("Empresario %d entra a la fila de empresas\n",cliente->id);
        sem_post(&fila_entrada);

        if(sem_trywait(&empleadoEmpresaDormido)==0){
            printf("El empresario %d despierta un empleado de empresa dormido\n",cliente->id);
            sem_post(&despertarEmpleadoEmpresa);
        }
        sem_wait(&pasarALaOficinaEmpresario);
        printf("Empresario %d pasa a la oficina\n",cliente->id);
        sem_wait(&atendido);
        printf("Empresario %d se retira\n",cliente->id);
    }
    else {
        printf("Empresario %d se retira ya que no hay lugar en la fila de entrada.\n", cliente->id);
    }
    free(cliente);
    exit(0);
}

void* clienteComun(void* arg){
    Cliente *cliente = (Cliente *)arg; //Casteo del argumento a cliente.

    if(sem_trywait(&fila_entrada) == 0) {
        printf("Cliente número %d esperando en fila de entrada\n", cliente->id);

        sem_wait(&fila_ClienteComun);
        sem_post(&CantComunesEsperando);
        printf("Cliente número %d entra a la fila de clientes comunes\n",cliente->id);
        sem_post(&fila_entrada);
        if(sem_trywait(&empleadoComunDormido)==0){
            sem_post(&despertarEmpleadoComun);
        }
        sem_wait(&pasarALaOficinaComun);
        printf("Cliente número %d pasa a la oficina\n",cliente->id);
        sem_wait(&atendido);
        printf("Cliente número %d se retira\n",cliente->id);
    }
    else {
        printf("Cliente número %d se retira ya que no hay lugar en la fila de entrada.\n", cliente->id);
    }
    free(cliente);
    exit(0);
}

int main() {
    //Inicio la cola de mensajes
    int queueID=0;
    key_t key;
    key = ftok("/tmp", 'A');
    queueID=msgget(key, 0666|IPC_CREAT);



    // Creación de procesos
    for (int i = 0; i < CANT_EMPLEADOS; i++) {
        int pid = fork();
        if (pid == 0) {
            int fila = i;
            if (i == 0) {
                empleadoComun(&fila);
            } else {
                empleadoEmpresa(&fila);
            }
            exit(0);
        }
    }

    for (int i = 0; i < CANT_PROCESOS; i++) {
        Cliente *cliente = (Cliente *)malloc(sizeof(Cliente));
        cliente->id = i;
        cliente->type = rand() % 3 + 1;
        int pid = fork();
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

    for (int i = 0; i < CANT_PROCESOS + CANT_EMPLEADOS; i++) {
        wait(NULL);
    }

    return 0;
}
