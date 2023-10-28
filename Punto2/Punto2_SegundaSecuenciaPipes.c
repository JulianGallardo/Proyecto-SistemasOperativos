#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int semA[2]; // Pipe para el semáforo del proceso A
int semB[2]; // Pipe para el semáforo del proceso B
int semC[2]; // Pipe para el semáforo del proceso C
int semD[2]; // Pipe para el semaforo del proceso D


void sem_Espera(int* sem) {//Simula un sem_wait con pipes.
    char buffer;
    read(sem[0], &buffer, 1); // Espera hasta que se escriba en el pipe
}

void sem_Libera(int* sem) {//Simula un sem_post con pipes.
    char buffer = 'x';
    write(sem[1], &buffer, 1); // Escribe en el pipe para liberar el semáforo
}

void processA() {
    while(1) {
        sem_Espera(semA); // Espera a que el semáforo A esté desbloqueado
        printf("A");
        fflush(stdout);
        sem_Libera(semB); // Libera el semáforo B para el proceso B
    }
}

void processB() {
    while (1) {
        sem_Espera(semB); // Espera a que el semáforo B esté desbloqueado
        printf("B");
        fflush(stdout);
        sem_Libera(semC); //Libera el semáforo C para el proceso C
        sem_Libera(semA);
    }
}

void processC() {
    while (1) {
        sem_Espera(semC);
        sem_Espera(semC); // Espera a que el semáforo C esté desbloqueado
        printf("C");
        fflush(stdout);

        sem_Espera(semC);
        printf("C");
        fflush(stdout);

        sem_Libera(semD);
    }
}

void processD() {
    while (1) {
        sem_Espera(semD); // Espera a que el semáforo D esté desbloqueado
        printf("D");
        fflush(stdout);

    }
}


int main() {
    // Inicializa los pipes (semáforos)
    if (pipe(semA) == -1 || pipe(semB) == -1 || pipe(semC) == -1 || pipe(semD) == -1 ) {
        perror("Error al crear los pipes");
        return 1;
    }

    // Crear los procesos
    pid_t pidA, pidB, pidC, pidD;

    pidA = fork();
    if (pidA == 0) {
        // Proceso hijo A
        processA();
    }

    pidB = fork();
    if (pidB == 0) {
        // Proceso hijo B
        processB();
    }

    pidC = fork();
    if (pidC == 0) {
        // Proceso hijo C
        processC();
    }

    pidD = fork();
    if (pidD == 0) {
        // Proceso hijo D
        processD();
    }
    //Inicializo los semaforos
    sem_Libera(semA);

    // Cerrar los extremos de escritura de los pipes en el proceso padre
    close(semA[1]);
    close(semB[1]);
    close(semC[1]);
    close(semD[1]);

    // Esperar a que los procesos hijos terminen (esto nunca sucederá en este caso)
    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);

    return 0;
}