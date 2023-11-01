#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>

#define MAX_CLIENTES_ENTRADA 30
#define MAX_CLIENTES_FILA 15
#define CANT_HILOS 80
#define CANT_EMPLEADOS 3


typedef struct {
	int id;
	int type; //1: Politico, 2: Empresa, 3: Cliente común.
} Cliente;


//Reemplazar todos los semaforos por pipes
sem_t fila_entrada; //Simula la mesa de entrada
sem_t fila_Politicos; //Simula la fila de espera de politicos.
sem_t fila_Empresa;//Simula la fila de espera de Empresas
sem_t fila_ClienteComun;//Simula la fila de espera de Clientes Comunes

sem_t CantPoliticosEsperando;//Cantidad de politicos esperando
sem_t CantEmpresasEsperando;//Cantidad de clientes empresarios esperando
sem_t CantComunesEsperando;//Cantidad de clientes comunes esperando

sem_t despertarEmpleadoEmpresa;
sem_t despertarEmpleadoComun;
sem_t empleadoEmpresaDormido;
sem_t empleadoComunDormido;

sem_t mutexEmpresa;
sem_t mutexPolitico;
sem_t mutexComun;

sem_t pasarALaOficinaEmpresario; //Semaforo que indica que el cliente empresario puede pasar a la oficina.
sem_t pasarALaOficinaPolitico; //Semaforo que indica que el cliente politico puede pasar a la oficina.
sem_t pasarALaOficinaComun; //Semaforo que indica que el cliente comun puede pasar a la oficina.

sem_t atendido; //Semaforo que indica que el cliente fue atendido por el empleado.

void *empleadoEmpresa(void *arg) {
	int fila_empleado = (int)arg;
    while(1) {

        sem_wait(&mutexPolitico);
		if(sem_trywait(&CantPoliticosEsperando) == 0) {
            sem_post(&pasarALaOficinaPolitico);
            sem_post(&mutexPolitico);
            sem_post(&fila_Politicos);
            usleep(1000);
            printf("Politico es atendido por el empleado %d\n", (fila_empleado + 1));
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
                printf("Cliente empresario es atendido por el empleado %d\n", (fila_empleado + 1));
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
            printf("Politico es atendido por el empleado %d\n", (fila_empleado + 1));
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
                printf("Cliente comun es atendido por el empleado %d\n", (fila_empleado + 1));
                sleep(1);
                printf("Cliente comun termino de ser atendido por el empleado %d\n", (fila_empleado + 1));
                sem_post(&atendido);
            }
        }


        sem_wait(&mutexComun);
        if(sem_trywait(&CantComunesEsperando)==-1){//Si no hay clientes de tipo empresa o de tipo politicos me duermo.
            sem_post(&mutexComun);
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
            sem_post(&mutexComun);
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

        sem_wait(&mutexEmpresa);
        sem_wait(&mutexPolitico);
        if(sem_trywait(&empleadoEmpresaDormido)==0){//Si hay un empleadoEmpresa dormido lo despierta
            sem_post(&despertarEmpleadoEmpresa);
        }
        else{//Si el empleadoEmpresa no esta dormido, intenta despertar al empleado comun.
            if(sem_trywait(&empleadoComunDormido)==0){//Si hay un empleadoComun dormido lo despierta
                sem_post(&despertarEmpleadoComun);
            }
        }
        sem_post(&mutexPolitico);
        sem_post(&mutexEmpresa);

        sem_wait(&pasarALaOficinaPolitico);
        printf("Politico %d pasa a la oficina\n",cliente->id);
        sem_wait(&atendido);
        printf("Politico %d se retira\n",cliente->id);
    }
    else {
        printf("Politico %d se retira ya que no hay lugar en la fila de entrada.\n", cliente->id);
    }

    free(cliente);
    pthread_exit(NULL);
}

void* clienteEmpresa(void* arg){
    Cliente *cliente = (Cliente *)arg; //Casteo del argumento a cliente.

    if(sem_trywait(&fila_entrada) == 0) {
        printf("Empresario %d esperando en fila de entrada\n", cliente->id);
        sem_wait(&fila_Empresa);
        sem_post(&CantEmpresasEsperando);
        printf("Empresario %d entra a la fila de empresas\n",cliente->id);
        sem_post(&fila_entrada);

        sem_wait(&mutexPolitico);
        sem_wait(&mutexEmpresa);
        if(sem_trywait(&empleadoEmpresaDormido)==0){
            printf("El empresario %d despierta un empleado de empresa dormido\n",cliente->id);
            sem_post(&despertarEmpleadoEmpresa);
        }
        sem_post(&mutexEmpresa);
        sem_post(&mutexPolitico);

        sem_wait(&pasarALaOficinaEmpresario);
        printf("Empresario %d pasa a la oficina\n",cliente->id);
        sem_wait(&atendido);
        printf("Empresario %d se retira\n",cliente->id);
    }
    else {
        printf("Empresario %d se retira ya que no hay lugar en la fila de entrada.\n", cliente->id);
    }
    free(cliente);
    pthread_exit(NULL);
}

void* clienteComun(void* arg){
    Cliente *cliente = (Cliente *)arg; //Casteo del argumento a cliente.

    if(sem_trywait(&fila_entrada) == 0) {
        printf("Cliente número %d esperando en fila de entrada\n", cliente->id);

        sem_wait(&fila_ClienteComun);
        sem_post(&CantComunesEsperando);
        printf("Cliente número %d entra a la fila de clientes comunes\n",cliente->id);
        sem_post(&fila_entrada);

        sem_wait(&mutexComun);
        sem_wait(&mutexPolitico);

        if(sem_trywait(&empleadoComunDormido)==0){
            sem_post(&despertarEmpleadoComun);
        }
        sem_post(&mutexPolitico);
        sem_post(&mutexComun);

        sem_wait(&pasarALaOficinaComun);
        printf("Cliente número %d pasa a la oficina\n",cliente->id);
        sem_wait(&atendido);
        printf("Cliente número %d se retira\n",cliente->id);
    }
    else {
        printf("Cliente número %d se retira ya que no hay lugar en la fila de entrada.\n", cliente->id);
    }
    free(cliente);
    pthread_exit(NULL);
}




int main() {
	//Inicialización semaforos
	sem_init(&fila_entrada, 0, MAX_CLIENTES_ENTRADA);
	sem_init(&fila_Empresa, 0, MAX_CLIENTES_FILA);
    sem_init(&fila_Politicos,0,MAX_CLIENTES_FILA);
    sem_init(&fila_ClienteComun,0,MAX_CLIENTES_FILA);
    sem_init(&CantEmpresasEsperando,0,0);
    sem_init(&CantComunesEsperando,0,0);
    sem_init(&CantPoliticosEsperando,0,0);
    sem_init(&despertarEmpleadoEmpresa,0,0);
    sem_init(&despertarEmpleadoComun,0,0);
    sem_init(&empleadoEmpresaDormido,0,0);
    sem_init(&empleadoComunDormido,0,0);
    sem_init(&mutexEmpresa,0,1);
    sem_init(&mutexPolitico,0,1);
    sem_init(&mutexComun,0,1);
    sem_init(&pasarALaOficinaEmpresario,0,0);
    sem_init(&pasarALaOficinaPolitico,0,0);
    sem_init(&pasarALaOficinaComun,0,0);
    sem_init(&atendido,0,0);



    //Inicialización hilos
	pthread_t hilos[CANT_HILOS];
	pthread_t hilos_empleados[3];
	for(int i = 0; i < CANT_EMPLEADOS; i++) {
		int fila = i;
		pthread_t hilo_empleado;
        if(i==0){
            pthread_create(&hilo_empleado, NULL, empleadoComun, (void *)fila);
        }
        else{
            pthread_create(&hilo_empleado, NULL, empleadoEmpresa, (void *)fila);
        }
		hilos_empleados[i] = hilo_empleado;
	}

    printf("Abre el banco\n");

	for(int i = 0; i < CANT_HILOS; i++) {
		Cliente *cliente = (Cliente *) malloc(sizeof(Cliente));
		cliente->id = i;
		cliente->type = rand() % 3 + 1;

		pthread_t hilo_cliente;
        switch(cliente->type) {
            case 1: //Politico
                pthread_create(&hilo_cliente, NULL, clientePolitico, (void *) cliente); break;
            case 2: //Empresa
                pthread_create(&hilo_cliente, NULL, clienteEmpresa, (void *) cliente); break;
            case 3: //Comun
                pthread_create(&hilo_cliente, NULL, clienteComun, (void *) cliente); break;
        }
		hilos[i] = hilo_cliente;
        if(i % 30==0){
            sleep(4);
        }
	}

	for(int i = 0; i < CANT_HILOS; i++) {
		pthread_join(hilos[i], NULL);
	}
	for(int i = 0; i < 3; i++) {
		pthread_cancel(hilos_empleados[i]);
	}

    printf("Cierra el banco");

	//Liberación de memoria semaforos
	sem_destroy(&fila_entrada);
    sem_destroy(&fila_Empresa);
    sem_destroy(&fila_Politicos);
    sem_destroy(&fila_ClienteComun);
    sem_destroy(&CantEmpresasEsperando);
    sem_destroy(&CantComunesEsperando);
    sem_destroy(&CantPoliticosEsperando);
    sem_destroy(&despertarEmpleadoEmpresa);
    sem_destroy(&despertarEmpleadoComun);
    sem_destroy(&empleadoEmpresaDormido);
    sem_destroy(&empleadoComunDormido);
    sem_destroy(&mutexEmpresa);
    sem_destroy(&mutexPolitico);
    sem_destroy(&mutexComun);
    sem_destroy(&pasarALaOficinaEmpresario);
    sem_destroy(&pasarALaOficinaPolitico);
    sem_destroy(&pasarALaOficinaComun);
    sem_destroy(&atendido);


	return 0;
}