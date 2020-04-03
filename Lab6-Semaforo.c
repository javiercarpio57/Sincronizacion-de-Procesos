#include <stdio.h> 
#include <pthread.h> 
#include <semaphore.h> 
#include <unistd.h> 

#define nthreads 5
#define nciclos 5

sem_t mutex;

void* doNothing (void* arg) {

    int j;
    printf("%d - Semaforo abierto con exito.\n", (int)pthread_self());

    for (j = 0; j < nciclos; j++) {
        sem_wait(&mutex);
        printf("Iniciando iteracion %d (%d)\n", j + 1, (int)pthread_self());
        printf("%d - (!) Recurso tomado.\n", (int)pthread_self());
        sleep(2);
        printf("%d - Holaaa, ya use el recurso.\n", (int)pthread_self());
        sem_post(&mutex);
        printf("%d - Ya devolvi el recurso :)\n", (int)pthread_self());
    }
}

int main() {
    printf("Iniciando programa.\n");
    pthread_t threads[nthreads];
    void * retvals[nthreads];
    int i;

    sem_init(&mutex, 1, 1);

    printf("Creando threads.\n");
    for (i = 0; i < nthreads; i++) {
        if (pthread_create(&threads[i], NULL, doNothing, NULL) != 0) {
            fprintf (stderr, "Error: No se puede crear thread # %d\n", i);
        } else {
            printf("Iniciando thread %d\n", (int)threads[i]);
        }
    }

    printf("Esperando threads.\n");
    for (i = 0; i < nthreads; i++) {
        if (pthread_join(threads[i], &retvals[i]) != 0) {
            fprintf (stderr, "Error: No se puede hacer join al thread #%d\n", i);
        }
    }

    sem_destroy(&mutex);

    return 0;
}