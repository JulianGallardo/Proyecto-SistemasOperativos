#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

//Semaforos
sem_t semA;
sem_t semB;
sem_t semC;
sem_t semD;


void* metodoA(){
    while(1){
        sem_wait(&semA);//decremento el semaforo A
        printf("A");
        sem_post(&semB);
    }
}

void* metodoB(){
    while(1){
        sem_wait(&semB);//decremento el semaforo B
        printf("B");
        sem_post(&semC);
        sem_post(&semA);

    }
}

void* metodoC(){
    while(1){
        sem_wait(&semC);//decremento el semaforo C
        sem_wait(&semC);//decremento el semaforo C
        printf("C");

        sem_wait(&semC);//decremento el semaforo C
        printf("C");
        sem_post(&semD);

    }
}

void* metodoD(){
    while(1){
        sem_wait(&semD);//decremento el semaforo D
        printf("D");
    }
}

int main(){
    //Inicializacion de los semaforos
    sem_init(&semA,0,1);  //Semaforo A
    sem_init(&semB,0,0);  //Semaforo B
    sem_init(&semC,0,0);  //Semaforo C
    sem_init(&semD,0,0);  //Semaforo D


    //Creación de hilos
    pthread_t hiloA;
    pthread_t hiloB;
    pthread_t hiloC;
    pthread_t hiloD;


    if (pthread_create( &hiloA, NULL, metodoA,NULL)) {
        printf("Error pthread_create()\n");
        exit(-1);
    }


    if (pthread_create( &hiloB, NULL, metodoB,NULL)) {
        printf("Error pthread_create()\n");
        exit(-1);
    }


    if (pthread_create( &hiloC, NULL, metodoC,NULL)) {
        printf("Error pthread_create()\n");
        exit(-1);
    }

    if (pthread_create( &hiloD, NULL, metodoD,NULL)) {
        printf("Error pthread_create()\n");
        exit(-1);
    }


    //Espero a que terminen los hilos para que no se muera el proceso.
    pthread_join(hiloA, NULL);
    pthread_join(hiloB, NULL);
    pthread_join(hiloC, NULL);
    pthread_join(hiloD, NULL);


    return 0;
}

