# include <stdio.h> 
# include <pthread.h>
# include <semaphore.h> 
# include <unistd.h>
# include <sys/syscall.h>

#define nthreads 5
#define nciclos 5

sem_t mutex;
int available_resources = 1;

void* doNothing (void* arg) {

    int j;
    pid_t tid;
    printf("%d - Semaforo abierto con exito.\n", tid);

    for (j = 0; j < nciclos; j++) {
        sem_wait(&mutex);

        printf("\tIniciando iteracion %d\n", j + 1);
        printf("\t%d - (!) Recurso tomado.\n", tid);
        available_resources--;

        int num = rand() % 3 + 1;
        sleep(num);

        printf("\t%d - Holaaa, ya use el recurso.\n", tid);
        available_resources++;

        sem_post(&mutex);

        printf("\t%d - Ya devolvi el recurso :)\n", tid);
    }
}

int main() {
    printf("Iniciando programa.\n");
    pthread_t threads[nthreads];
    void * retvals[nthreads];
    int i;

    sem_init(&mutex, 0, 1);

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