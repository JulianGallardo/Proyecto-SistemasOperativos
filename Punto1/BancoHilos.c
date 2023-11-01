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



int numeroRandom(int adicion){
    // Semilla para la generación de números aleatorios

    srand(time(NULL)+adicion);
    int result =(int)rand();
    return result;
}


void *empleadoEmpresa(void *arg) {
	int fila_empleado = (int)arg;
    while(1) {

        sem_wait(&mutexPolitico);//Tomo el mutex Politico
		if(sem_trywait(&CantPoliticosEsperando) == 0) {//Si hay politicos esperando, atiendo a uno
            sem_post(&pasarALaOficinaPolitico);//Le indico que puede pasar a la oficina
            sem_post(&mutexPolitico);//Libero el mutex de politicos
            sem_post(&fila_Politicos);//Aviso que se libero el lugar del politico en su fila.
            usleep(1000);
            printf("Politico es atendido por el empleado %d\n", (fila_empleado + 1));
            sleep(1);
            printf("Politico termino de ser atendido por el empleado %d\n", (fila_empleado + 1));
            sem_post(&atendido);//Indico que el politico fue atendido.
		}
		else {
            sem_post(&mutexPolitico);//Libero el mutex de politicos
            sem_wait(&mutexEmpresa);//Tomo el mutex de empresa
            if(sem_trywait(&CantEmpresasEsperando)==0) {//Si hay empresas esperando, atiendo a una
                sem_post(&pasarALaOficinaEmpresario);//Le indico que puede pasar a la oficina
                sem_post(&mutexEmpresa);//Libero el mutex de empresa
                sem_post(&fila_Empresa);//Aviso que se libero el lugar del empresario en su fila.
                usleep(1000);
                printf("Cliente empresario es atendido por el empleado %d\n", (fila_empleado + 1));
                sleep(1);
                printf("Cliente empresario termino de ser atendido por el empleado %d\n", (fila_empleado + 1));
                sem_post(&atendido);//Indico que el empresario fue atendido.
            }
            else{
                sem_post(&mutexEmpresa);//Libero el mutex de empresa
            }
		}

        sem_wait(&mutexEmpresa);//Tomo el mutex de empresa
        if(sem_trywait(&CantEmpresasEsperando)==-1){//Si no hay clientes de tipo empresa o de tipo politicos me duermo.
            sem_post(&mutexEmpresa);//Libero el mutex de empresa
            sem_wait(&mutexPolitico);//Tomo el mutex de politicos
            if(sem_trywait(&CantPoliticosEsperando)==-1){//Si no hay clientes de tipo politico o de tipo empresa me duermo.
                sem_post(&mutexPolitico);//Libero el mutex de politicos
                sem_post(&empleadoEmpresaDormido);//Indico que el empleado se durmio.
                printf("El empleado de empresa %d se duerme\n",fila_empleado+1);
                sem_wait(&despertarEmpleadoEmpresa);//Espero a que me despierten.
            }
            else{//Si hay clientes de tipo politico, devuelvo el politico sacado y el mutex.
                sem_post(&CantPoliticosEsperando);
                sem_post(&mutexPolitico);
            }

        }
        else{//Si hay clientes de tipo empresa, devuelvo el empresario sacado y el mutex.
            sem_post(&CantEmpresasEsperando);
            sem_post(&mutexEmpresa);
        }

	}
}

void *empleadoComun(void *arg) {
    int fila_empleado = (int)arg;

    while(1){

        sem_wait(&mutexPolitico);
        if(sem_trywait(&CantPoliticosEsperando) == 0) {//Si hay politicos esperando, atiendo a uno
            sem_post(&pasarALaOficinaPolitico);//Le indico que puede pasar a la oficina
            sem_post(&mutexPolitico);//Libero el mutex de politicos
            sem_post(&fila_Politicos);//Aviso que se libero el lugar del politico en su fila.
            usleep(1000);
            printf("Politico es atendido por el empleado %d\n", (fila_empleado + 1));
            sleep(1);
            printf("Politico termino de ser atendido por el empleado %d\n", (fila_empleado + 1));
            sem_post(&atendido);//Indico que el politico fue atendido.
        }
        else {
            sem_post(&mutexPolitico);//Libero el mutex de politicos
            if(sem_trywait(&CantComunesEsperando)==0) {//Si hay clientes comunes esperando, atiendo a uno
                sem_post(&pasarALaOficinaComun);//Le indico que puede pasar a la oficina
                sem_post(&fila_ClienteComun);//Aviso que se libero el lugar del cliente comun en su fila.
                usleep(1000);
                printf("Cliente comun es atendido por el empleado %d\n", (fila_empleado + 1));
                sleep(1);
                printf("Cliente comun termino de ser atendido por el empleado %d\n", (fila_empleado + 1));
                sem_post(&atendido);//Indico que el cliente comun fue atendido.
            }
        }


        sem_wait(&mutexComun);//Tomo el mutex de comun
        if(sem_trywait(&CantComunesEsperando)==-1){//Si no hay clientes de tipo empresa o de tipo politicos me duermo.
            sem_post(&mutexComun);//Libero el mutex de comun
            sem_wait(&mutexPolitico);//Tomo el mutex de politicos
            if(sem_trywait(&CantPoliticosEsperando)==-1){//Si no hay clientes de tipo politico o de tipo empresa me duermo.
                sem_post(&empleadoComunDormido);//Indico que el empleado se durmio.
                sem_post(&mutexPolitico);//Libero el mutex de politicos
                printf("El empleado comun %d se duerme\n",fila_empleado+1);
                sem_wait(&despertarEmpleadoComun);//Espero a que me despierten.
            }
            else{//Si hay clientes de tipo politico, devuelvo el politico sacado y el mutex.
                sem_post(&CantPoliticosEsperando);
                sem_post(&mutexPolitico);
            }
        }
        else{//Si hay clientes de tipo empresa, devuelvo el empresario sacado y el mutex.
            sem_post(&CantComunesEsperando);
            sem_post(&mutexComun);
        }
    }
}

void* clientePolitico(void* arg){
    Cliente *cliente = (Cliente *)arg; //Casteo del argumento a cliente.

    if(sem_trywait(&fila_entrada) == 0) {//Si hay lugar en la fila de entrada
        printf("Politico %d esperando en fila de entrada\n", cliente->id);

        sem_wait(&fila_Politicos);//Esperamos un lugar en la fila de politicos
        sem_post(&CantPoliticosEsperando);//Indicamos que hay un politico esperando
        printf("Politico %d entra a la fila de politicos\n",cliente->id);
        sem_post(&fila_entrada);//Libero mi lugar en la fila de entrada

        sem_wait(&mutexEmpresa);//Tomo el mutex Empresa
        sem_wait(&mutexPolitico);//Tomo el mutex Politico
        if(sem_trywait(&empleadoEmpresaDormido)==0){//Si hay un empleadoEmpresa dormido lo despierta
            sem_post(&despertarEmpleadoEmpresa);
        }
        else{//Si el empleadoEmpresa no esta dormido, intenta despertar al empleado comun.
            if(sem_trywait(&empleadoComunDormido)==0){//Si hay un empleadoComun dormido lo despierta
                sem_post(&despertarEmpleadoComun);
            }
        }
        sem_post(&mutexPolitico);//Libero el mutex Politico
        sem_post(&mutexEmpresa);//Libero el mutex Empresa

        sem_wait(&pasarALaOficinaPolitico);//Espero a que me indiquen que puedo pasar a la oficina
        printf("Politico %d pasa a la oficina\n",cliente->id);
        sem_wait(&atendido);//Espero a que me indiquen que fui atendido
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

    if(sem_trywait(&fila_entrada) == 0) {//Si hay lugar en la fila de entrada
        printf("Empresario %d esperando en fila de entrada\n", cliente->id);
        sem_wait(&fila_Empresa); //Esperamos un lugar en la fila de empresas
        sem_post(&CantEmpresasEsperando); //Indicamos que hay un empresario esperando
        printf("Empresario %d entra a la fila de empresas\n",cliente->id);
        sem_post(&fila_entrada); //Libero mi lugar en la fila de entrada

        sem_wait(&mutexPolitico);//Tomo el mutex Politico
        sem_wait(&mutexEmpresa);//Tomo el mutex Empresa
        if(sem_trywait(&empleadoEmpresaDormido)==0){//Si hay un empleadoEmpresa dormido lo despierta
            printf("El empresario %d despierta un empleado de empresa dormido\n",cliente->id);
            sem_post(&despertarEmpleadoEmpresa);
        }
        sem_post(&mutexEmpresa);//Libero el mutex Empresa
        sem_post(&mutexPolitico);//Libero el mutex Politico

        sem_wait(&pasarALaOficinaEmpresario);//Espero a que me indiquen que puedo pasar a la oficina
        printf("Empresario %d pasa a la oficina\n",cliente->id);
        sem_wait(&atendido);//Espero a que me indiquen que fui atendido
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

    if(sem_trywait(&fila_entrada) == 0) {//Si hay lugar en la fila de entrada
        printf("Cliente número %d esperando en fila de entrada\n", cliente->id);

        sem_wait(&fila_ClienteComun);//Esperamos un lugar en la fila de clientes comunes
        sem_post(&CantComunesEsperando);//Indicamos que hay un cliente comun esperando
        printf("Cliente número %d entra a la fila de clientes comunes\n",cliente->id);
        sem_post(&fila_entrada);//Libero mi lugar en la fila de entrada

        sem_wait(&mutexComun);//Tomo el mutex Comun
        sem_wait(&mutexPolitico);//Tomo el mutex Politico

        if(sem_trywait(&empleadoComunDormido)==0){//Si hay un empleadoComun dormido lo despierta
            sem_post(&despertarEmpleadoComun);
        }
        sem_post(&mutexPolitico);//Libero el mutex Politico
        sem_post(&mutexComun);//Libero el mutex Comun

        sem_wait(&pasarALaOficinaComun);//Espero a que me indiquen que puedo pasar a la oficina
        printf("Cliente número %d pasa a la oficina\n",cliente->id);
        sem_wait(&atendido);//Espero a que me indiquen que fui atendido
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
		cliente->type = numeroRandom(i) % 100 + 1;

		pthread_t hilo_cliente;
        if(cliente->type<=25) {
            //Politico
            pthread_create(&hilo_cliente, NULL, clientePolitico, (void *) cliente);
        }
        else {
            if (cliente->type <= 65) {
                //Empresa
                pthread_create(&hilo_cliente, NULL, clienteEmpresa, (void *) cliente);
            } else {
                //Comun
                pthread_create(&hilo_cliente, NULL, clienteComun, (void *) cliente);
            }
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