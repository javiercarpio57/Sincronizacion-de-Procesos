# include <stdio.h> 
# include <pthread.h>
# include <semaphore.h> 
# include <unistd.h>
# include <stdlib.h>
# include <string.h>
# include <sys/syscall.h>

#define nthreads 5
#define nciclos 5

sem_t mutex;
int available_resources = 1;
FILE * fp;

void* doNothing (void* arg) {
    int j;
    pid_t tid;
    tid = syscall(SYS_gettid);

    fprintf(fp, "Iniciando thread %d.\n", tid);

    printf("%d - Semaforo abierto con exito.\n", tid);
    fprintf(fp, "%d - Semaforo abierto con exito.\n", tid);

    for (j = 0; j < nciclos; j++) {
        sem_wait(&mutex);

        printf("Iniciando iteracion %d\n", j + 1);
        fprintf(fp, "Iniciando iteracion %d.\n", j + 1);

        printf("\t%d - (!) Recurso tomado.\n", tid);
        fprintf(fp, "\t%d - (!) Recurso tomado.\n", tid);

        available_resources--;

        int num = rand() % 3 + 1;
        sleep(num);

        printf("\t%d - Holaaa, ya use el recurso.\n", tid);
        fprintf(fp, "\t%d - Holaaa, ya use el recurso.\n", tid);

        available_resources++;

        sem_post(&mutex);

        printf("\t%d - Ya devolvi el recurso :)\n", tid);
        fprintf(fp, "\t%d - Ya devolvi el recurso :)\n", tid);
    }
}

int main() {
    fp = fopen("SemaforoBitacora.txt", "w");

    printf("Iniciando programa.\n");
    fprintf(fp, "Iniciando programa.\n");

    pthread_t threads[nthreads];
    void * retvals[nthreads];
    int i;

    sem_init(&mutex, 0, 1);

    printf("Creando threads.\n");
    fprintf(fp, "Creando thread.\n");
    for (i = 0; i < nthreads; i++) {
        if (pthread_create(&threads[i], NULL, doNothing, NULL) != 0) {
            fprintf (stderr, "Error: No se puede crear thread # %d\n", i);
        }
    }

    printf("Esperando threads.\n");
    fprintf(fp, "Esperando thread.\n");
    for (i = 0; i < nthreads; i++) {
        if (pthread_join(threads[i], &retvals[i]) != 0) {
            fprintf (stderr, "Error: No se puede hacer join al thread #%d\n", i);
        }
    }

    sem_destroy(&mutex);
    fclose(fp);
    return 0;
}