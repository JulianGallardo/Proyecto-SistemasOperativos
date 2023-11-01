#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

#define CANT_HILOS 25
#define CANT_HORARIOS 12
#define CANT_CONSULTAS 4

//Tabla de Reservas
int horarios[CANT_HORARIOS]; //Los horarios son de 9:00 a 21:00

//Semaforos
sem_t semAccesoReserva;
sem_t cantConsultas;
sem_t mutex;

int numeroRandom(int adicion){
    // Semilla para la generación de números aleatorios

    srand(time(NULL)+adicion);
    int result =(int)rand();
    return result;
}

void reservarAula(int idAlumno){
    int horarioAReservar=(numeroRandom(idAlumno) % 12);
    sem_wait(&semAccesoReserva);//Espero a poder acceder a la reserva
    if(horarios[horarioAReservar]==0) {
        horarios[horarioAReservar] = idAlumno;
        printf("El alumno %d reservo el horario %d\n",idAlumno,horarioAReservar+9);
    }
    else{
        printf("El alumno %d no pudo reservar el horario %d\n",idAlumno,horarioAReservar+9);
    }
    sem_post(&semAccesoReserva);//Libero el semaforo de reservas
}

void cancelarReserva(int idAlumno){
    int horarioAReservar=(numeroRandom(idAlumno) % 12);
    sem_wait(&semAccesoReserva);//Espero a poder acceder a la reserva
    if(horarios[horarioAReservar]==idAlumno) {
        horarios[horarioAReservar] = 0;
        printf("El alumno %d cancelo la reserva del horario %d \n",idAlumno,horarioAReservar+9);
    }
    else{
        printf("El alumno %d no pudo cancelar la reserva del horario %d \n",idAlumno,horarioAReservar+9);
    }
    sem_post(&semAccesoReserva);//Libero el semaforo de reservas
}

void consultarHorariosReserva(int idAlumno){
    int i= numeroRandom(idAlumno*2) % 12;
    sem_wait(&mutex);//Espero a poder acceder al mutex
    if(sem_trywait(&cantConsultas)==0){ //Vemos si hay gente consultando ya.
        sem_post(&cantConsultas);
        sem_post(&mutex);
    }
    else{//Si no hay gente consultando intentamos tomar el acceso a la tabla de reservas.
        sem_wait(&semAccesoReserva);
        sem_post(&cantConsultas);
        sem_post(&mutex);
    }

    if(horarios[i]!=0){
        printf("El alumno %d consulta si esta reservado el horario %d (Estado: Reservado por alumno %d)\n",idAlumno,i+9,horarios[i]);
    }
    else {
        printf("El alumno %d consulta si esta reservado el horario %d (Estado: No reservado)\n", idAlumno, i + 9);
    }



    sem_wait(&cantConsultas);//Indico que termine la consulta
    sem_wait(&mutex);//Espero a tomar el mutex de consultas
    if(sem_trywait(&cantConsultas)==0){//Si todavia queda gente consultando, no hago nada
        sem_post(&cantConsultas);
        sem_post(&mutex);
    }
    else{//Si soy la ultima consulta libero el Acceso a la tabla de reservas.
        sem_post(&semAccesoReserva);
        sem_post(&mutex);
    }

}

void* Alumno(void* idAlumnoPuntero){
    int* idAlumno=(int*)idAlumnoPuntero;
    int decision;
    int i;
    for(i=0;i<CANT_CONSULTAS;i++){//El alumno realiza una consulta, debe realizar 4 de ellas
        decision=(numeroRandom(*idAlumno) % 4);
        if(decision<2){
            reservarAula(*idAlumno);
        }
        else if(decision==2){
            cancelarReserva(*idAlumno);
        }
        else{
            consultarHorariosReserva(*idAlumno);
        }
        sleep(1);
    }
    pthread_exit(NULL);
}



int main(){
    //Inicializacion de los semaforos
    sem_init(&semAccesoReserva,0,1);  //Semaforo Reservas
    sem_init(&cantConsultas,0,0);  //Semaforo cantidad consultas
    sem_init(&mutex,0,1);  //Semaforo mutex
    //Creación de hilos
    pthread_t hilos[CANT_HILOS];

    int idAlumnos[CANT_HILOS];
    int i;

    for(i=0;i<CANT_HORARIOS;i++){
        horarios[i]=0;
    }


    for(i=0;i<CANT_HILOS;i++) {
        idAlumnos[i]=i+1;
        if (pthread_create(&hilos[i], NULL, Alumno, &idAlumnos[i])) {
            printf("Error pthread_create()\n");
            exit(-1);
        }
    }
    //Espero a que terminen los hilos para que no se muera el proceso.
    for(i=0;i<CANT_HILOS;i++) {
        pthread_join(hilos[i], NULL);
    }

    sem_destroy(&semAccesoReserva);
    sem_destroy(&cantConsultas);
    sem_destroy(&mutex);

    return 0;
}



