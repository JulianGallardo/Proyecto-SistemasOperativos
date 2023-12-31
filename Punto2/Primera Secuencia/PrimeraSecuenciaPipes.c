#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int semA[2]; // Pipe para el semáforo del proceso A
int semB[2]; // Pipe para el semáforo del proceso B
int semC[2]; // Pipe para el semáforo del proceso C

void sem_Espera(int* sem) {//Simula un sem_wait con pipes.
    char buffer;
    read(sem[0], &buffer, 1);
}

void sem_Envia(int* sem) {//Simula un sem_post con pipes.
    char buffer = 'x';
    write(sem[1], &buffer, 1);
}

void processA() {
    while(1) {
        sem_Espera(semA); // Espera a que el semáforo A esté desbloqueado
        printf("A");
        fflush(stdout);
        sem_Envia(semB); // Libera el semáforo B para el proceso B
    }
}

void processB() {
    while (1) {
        sem_Espera(semB); // Espera a que el semáforo B esté desbloqueado
        printf("B");
        fflush(stdout);
        sem_Envia(semC);
        sem_Envia(semA);
    }
}

void processC() {
    while (1) {
        sem_Espera(semC);
        sem_Espera(semC);
        printf("C");
        fflush(stdout);
    }
}

int main() {
    // Inicializa los pipes (semáforos)
    if (pipe(semA) == -1 || pipe(semB) == -1 || pipe(semC) == -1) {
        perror("Error al crear los pipes");
        return 1;
    }

    // Crear los procesos
    pid_t pidA, pidB, pidC;

    pidA = fork();
    if (pidA == 0) {
        // Proceso hijo AR
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


    //Inicializo los semaforos
    sem_Envia(semA);

    // Cerrar los extremos de escritura de los pipes en el proceso padre
    close(semA[1]);
    close(semB[1]);
    close(semC[1]);

    // Esperar a que los procesos hijos terminen (esto nunca sucederá en este caso)
    wait(NULL);
    wait(NULL);
    wait(NULL);
    return 0;
}