# include <unistd.h>
# include <stdlib.h>
# include <sys/syscall.h>
# include <stdio.h>
# include <pthread.h>

#define nthreads 10
#define nciclos 5
#define maxRecursos 20

pthread_mutex_t mutex;
pthread_cond_t condition_variable;
unsigned available_resources;
FILE * fp;

int decrease_count (int count) {
    pid_t tid = syscall(SYS_gettid);
    fprintf(fp, "Entrando a decrease_count (%d).\n", tid);    

    pthread_mutex_lock (&mutex);

    fprintf(fp, "\t%d - Mutex adquirido, entrando al monitor.\n", tid);

    while (available_resources < count) {
        fprintf(fp, "\tEsperando por %d recursos - %d\n", count, tid);
        pthread_cond_wait (&condition_variable, &mutex);
    }

    fprintf(fp, "\t%d - Recursos suficientes disponibles (%d/%d), consumiendo...\n", tid, count, available_resources);
    available_resources -= count;
    fprintf(fp, "\t%d - (!) Recurso tomado.\n", tid);

    pthread_mutex_unlock (&mutex);
    fprintf(fp, "Saliendo de decrease_count (%d).\n", tid);
}

void* increase_count (int count) {
    pid_t tid = syscall(SYS_gettid);
    fprintf(fp, "Entrando a increase_count (%d).\n", tid);
    
    pthread_mutex_lock (&mutex);
    if (available_resources <= 20) {
        pthread_cond_signal (&condition_variable);
        fprintf(fp, "%d - Avisando a la mara (%d).\n", tid, available_resources);
    }
    available_resources += count;
    fprintf(fp, "\t%d - Recurso usado.\n", tid);

    pthread_mutex_unlock (&mutex);
    fprintf(fp, "Saliendo de increase_count.\n");

    fprintf(fp, "Recursos disponibles: %d.\n", available_resources);
}

void* consume (void* arg) {
    int i;
    pid_t tid = syscall(SYS_gettid);
    fprintf(fp, "Iniciando thread %d\n", tid);

    for (i = 0; i < nciclos; i++) {
        fprintf(fp, "\t(%d) Iniciando iteracion #%d.\n", tid, i + 1);
        int num = rand() % maxRecursos + 1;
        fprintf(fp, "\t(%d) Se consumiran %d recursos.\n", tid, num);

        decrease_count(num);
        increase_count(num);
    }
}

int main () {
    fp = fopen("MonitorBitacora.txt", "w");
    fprintf(fp, "Iniciando programa.\n");
    pthread_t threads[nthreads];
    void * retvals[nthreads];
    int i;

    int ret = pthread_cond_init(&condition_variable, NULL);
    available_resources = maxRecursos;

    if (ret == 0) {
        fprintf(fp, "Creando threads.\n");
        for (i = 0; i < nthreads; i++) {
            if (pthread_create(&threads[i], NULL, consume, NULL) != 0) {
                fprintf (stderr, "Error: No se puede crear thread # %d\n", i);
            }
        }

        fprintf(fp, "Esperando threads.\n");
        for (i = 0; i < nthreads; i++) {
            if (pthread_join(threads[i], &retvals[i]) != 0) {
                fprintf (stderr, "Error: No se puede hacer join al thread #%d\n", i);
            }
        }
    }
    fprintf(fp, "Adios.\n");

    pthread_cond_destroy(&condition_variable);

    return 0;
}