#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>


#define CANT_PROCESOS 25
#define CANT_HORARIOS 12
#define CANT_CONSULTAS 4
#define KEY ((key_t) (1243))

//Struct para la memoria compartida
struct compartido{
    int horarios[CANT_HORARIOS]; //Los horarios son de 9:00 a 21:00
    sem_t semAccesoReserva;
    sem_t cantConsultas;
    sem_t mutex;
};
int numeroRandom(int adicion){
    // Semilla para la generación de números aleatorios

    srand(time(NULL)+adicion);
    int result =(int)rand();
    return result;
}

void reservarAula(int idAlumno,struct compartido* compartido){
    int horarioAReservar=(numeroRandom(idAlumno) % 12);
    sem_wait(&compartido->semAccesoReserva);
    if(compartido->horarios[horarioAReservar]==0) {
        compartido->horarios[horarioAReservar] = idAlumno;
        printf("El alumno %d reservo el horario %d\n",idAlumno,horarioAReservar+9);
    }
    else{
        printf("El alumno %d no pudo reservar el horario %d\n",idAlumno,horarioAReservar+9);
    }
    sem_post(&compartido->semAccesoReserva);
}

void cancelarReserva(int idAlumno,struct compartido* compartido){
    int horarioAReservar=(numeroRandom(idAlumno) % 12);
    sem_wait(&compartido->semAccesoReserva);
    if(compartido->horarios[horarioAReservar]==idAlumno) {
        compartido->horarios[horarioAReservar] = 0;
        printf("El alumno %d cancelo la reserva del horario %d \n",idAlumno,horarioAReservar+9);
    }
    else{
        printf("El alumno %d no pudo cancelar la reserva del horario %d \n",idAlumno,horarioAReservar+9);
    }
    sem_post(&compartido->semAccesoReserva);
}

void consultarHorariosReserva(int idAlumno,struct compartido* compartido){
    int i= numeroRandom(idAlumno*2) % 12;
    sem_wait(&compartido->mutex);
    if(sem_trywait(&compartido->cantConsultas)==0){ //Vemos si hay gente consultando ya.
        sem_post(&compartido->cantConsultas);
        sem_post(&compartido->mutex);
    }
    else{//Si no hay gente consultando intentamos tomar el mutex.
        sem_wait(&compartido->semAccesoReserva);
        sem_post(&compartido->cantConsultas);
        sem_post(&compartido->mutex);
    }

    if(compartido->horarios[i]!=0){
        printf("El alumno %d consulta si esta reservado el horario %d (Estado: Reservado por alumno %d)\n",idAlumno,i+9,compartido->horarios[i]);
    }
    else {
        printf("El alumno %d consulta si esta reservado el horario %d (Estado: No reservado)\n", idAlumno, i + 9);
    }



    sem_wait(&compartido->cantConsultas);
    sem_wait(&compartido->mutex);
    if(sem_trywait(&compartido->cantConsultas)==0){//Si todavia queda gente consultando, no hago nada
        sem_post(&compartido->cantConsultas);
        sem_post(&compartido->mutex);
    }
    else{//Si soy la ultima consulta libero el mutex.
        sem_post(&compartido->semAccesoReserva);
        sem_post(&compartido->mutex);
    }

}

void* Alumno(void* idAlumnoPuntero){
    struct compartido* compartido;
    int tam = sizeof(struct compartido);
    int id = shmget(KEY, tam, IPC_CREAT | 0666);
    compartido = (struct compartido *) shmat(id, 0, 0);
    int* idAlumno=(int*)idAlumnoPuntero;
    int decision,i;
    for(i=0;i<CANT_CONSULTAS;i++){
        decision=(numeroRandom(*idAlumno) % 4);
        if(decision<2){
            reservarAula(*idAlumno,compartido);
        }
        else if(decision==2){
            cancelarReserva(*idAlumno,compartido);
        }
        else{
            consultarHorariosReserva(*idAlumno,compartido);
        }
        sleep(1);
    }
    exit(0);
}



int main(){
    struct compartido* compartido;
    int tam = sizeof(struct compartido);
    int id = shmget(KEY, tam, IPC_CREAT | 0666);
    compartido = (struct compartido *) shmat(id, 0, 0);

    //Inicializacion de los semaforos
    sem_init(&compartido->semAccesoReserva,1,1);  //Semaforo Reservas
    sem_init(&compartido->cantConsultas,1,0);  //Semaforo cantidad consultas
    sem_init(&compartido->mutex,1,1);  //Semaforo mutex (para la cantidad de consultas
    //Creación de hilos
    

    int idAlumnos[CANT_PROCESOS];
    int i;
    pid_t pid;

    for(i=0;i<CANT_HORARIOS;i++){
        compartido->horarios[i]=0;
    }


    for(i=0;i<CANT_PROCESOS;i++) {
        idAlumnos[i]=i+1;
        pid = fork();
        if(pid==-1){
            printf("Error al crear el proceso hijo\n");
            exit(-1);
        }
        else if (pid == 0) {
            Alumno(&idAlumnos[i]);
        }
    }
    //Espero a que terminen los hilos para que no se muera el proceso.
    for(i=0;i<CANT_PROCESOS;i++) {
        wait(NULL);
    }

    sem_destroy(&compartido->semAccesoReserva);
    sem_destroy(&compartido->cantConsultas);
    sem_destroy(&compartido->mutex);

    // Liberar la memoria compartida
    shmdt(compartido);
    shmctl(id, IPC_RMID, NULL);
    return 0;
}



