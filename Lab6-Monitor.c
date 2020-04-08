# include <unistd.h>
# include <stdlib.h>
# include <sys/syscall.h>
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

struct condition_variable {
    sem_t semaforo;
    int count;
};

struct Monitor monitor;
struct condition_variable x;

// x.wait()
void wait () {
    x.count++;

    if (monitor.next_count > 0)
        sem_post(&monitor.next);
    else
        sem_post(&monitor.mutex);

    int status = sem_wait(&(x.semaforo));
    printf("STATUS: %d\n", status);
    x.count--;
}

// x.signal()
void signal () {
    if (x.count > 0) {
        monitor.next_count++;
        sem_post (&(x.semaforo));
        sem_wait (&monitor.next);
        monitor.next_count--;
    }
}

int initialize_monitor () {
    int return_value = 1;

    monitor.available_resources = maxRecursos;
    sem_init(&(x.semaforo), 0, 0);

    if ((sem_init(&(monitor.mutex), 0, 1) == 0) && (sem_init(&(monitor.next), 0, 0) == 0))
        return_value = 0;
    else
        printf("Unable to initialize semaphores\n");

    return return_value;
}

int decrease_count (int count) {
    pid_t tid = syscall(SYS_gettid);

    printf("\t(%d) Iniciando decrease_count.\n", tid);

    sem_wait(&(monitor.mutex));
    printf("\tMutex adquirido, entrando a monitor.\n");

    if (monitor.available_resources < count) {
        return -1;
    } else {
        monitor.available_resources -= count;
        return 0;
    }

    if (monitor.next_count > 0)
        sem_post(&(monitor.next));
    else
        sem_post(&(monitor.mutex));
}

void* increase_count (int count) {
    pid_t tid = syscall(SYS_gettid);
    printf("\t(%d) Iniciando increase_count.\n", tid);

    sem_wait(&(monitor.mutex));

    printf("%d - Holaaa, ya use el recurso.\n", tid);
    monitor.available_resources += count;

    if (monitor.next_count > 0) {
        printf("---- Next ----\n");
        sem_post(&monitor.next);
    } else {
        printf("---- Mutex ----\n");
        sem_post(&(monitor.mutex));
    }
}

void* consume (void* arg) {
    int i;

    for (i = 0; i < nciclos; i++) {
        printf("\tIniciando iteracion #%d.\n", i + 1);

        pid_t tid = syscall(SYS_gettid);
        int num = rand() % 5 + 1;
        printf("\t(%d) Se consumiran %d recursos.\n", tid, num);

        wait();
        decrease_count(num);
        increase_count(num);
        signal();
    }
}

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
                printf("Iniciando thread %d\n", i + 1);
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