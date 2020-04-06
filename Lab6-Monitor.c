# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <pthread.h>
# include <semaphore.h>

#define nthreads 2
#define nciclos 5
#define maxRecursos 10

struct Monitor {
    sem_t mutex;
    sem_t next;
    int next_count;

    int available_resources;
};

struct Monitor monitor;

int main () {
    printf("Iniciando programa.\n");
    pthread_t threads[nthreads];
    void * retvals[nthreads];
    int i;

    if (initialize_monitor() == 0) {
        printf("Creando threads.\n");
        for (i = 0; i < nthreads; i++) {
            if (pthread_create(&threads[i], NULL, consume, NULL) != 0) {
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
    }

    return 0;
}

int initialize_monitor () {
    int return_value = 1;

    monitor.available_resources = maxRecursos;

    if ((sem_init(&(monitor.mutex), 0, 1) == 0) && (sem_init(&(monitor.next), 0, 0) == 0))
        return_value = 0;
    else
        printf("Unable to initialize semaphores\n");

    return return_value;
}

void* consume (void* arg) {
    int i;

    for (i = 0; i < nciclos; i++) {
        printf("\tIniciando iteracion #%d.\n", i + 1);

        int num = rand() % 5 + 1;
        printf("\tSe consumiran %d recursos.\n", num);

        decrease_count(num);
        increase_count(num);
    }
}

void decrease_count (int count) {
    printf("\tIniciando decrease_count.\n");

    sem_wait(&(monitor.mutex));
    printf("\tMutex adquirido, entrando a monitor.\n");

    if (monitor.available_resources >= count) {
        printf("\tRecursos sufiecientes, consumiendo...\n");

        printf("%d - (!) Recurso tomado.\n", (int)pthread_self());
        monitor.available_resources -= count;

        sleep(2);
    }

    if (monitor.next_count > 0)
        sem_post(&(monitor.next));
    else
        sem_post(&(monitor.mutex));
}

void increase_count (int count) {
    printf("\tIniciando increase_count.\n");

    sem_wait(&(monitor.mutex));

    printf("%d - Holaaa, ya use el recurso.\n", (int)pthread_self());
    monitor.available_resources += count;

    if (monitor.next_count > 0)
        sem_post(&monitor.next);
    else
        sem_post(&(monitor.mutex));
}