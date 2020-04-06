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
char txtContent[2048];
FILE * fp;

void* doNothing (void* arg) {

    int j;
    pid_t tid;
    tid = syscall(SYS_gettid);

    char mes[50] = {0};
    char t[10] = {0};
    sprintf(t, "%d", tid);
    strcat(mes, t);
    strcat(mes, " - Semaforo abierto con exito.\n");
    strcat(txtContent, mes);

    printf("%d - Semaforo abierto con exito.\n", tid);

    for (j = 0; j < nciclos; j++) {
        sem_wait(&mutex);

        printf("Iniciando iteracion %d\n", j + 1);
        char mes[50] = {0};
        char t[5] = {0};
        sprintf(t, "%d", j + 1);
        strcat(mes, "Iniciando iteracion ");
        strcat(mes, t);
        strcat(mes, "\n");
        strcat(txtContent, mes);

        printf("\t%d - (!) Recurso tomado.\n", tid);
        char mes2[50] = {0};
        char t2[10] = {0};
        sprintf(t2, "%d", tid);
        strcat(mes2, "\t");
        strcat(mes2, t2);
        strcat(mes2, " - (!) Recurso tomado.\n");
        strcat(txtContent, mes2);
        
        available_resources--;

        int num = rand() % 3 + 1;
        sleep(num);

        printf("\t%d - Holaaa, ya use el recurso.\n", tid);
        char mes3[50] = {0};
        strcat(mes3, "\t");
        strcat(mes3, t2);
        strcat(mes3, " - Holaaa, ya use el recurso.\n");
        strcat(txtContent, mes3);

        available_resources++;

        sem_post(&mutex);

        printf("\t%d - Ya devolvi el recurso :)\n", tid);
        char mes4[50] = {0};
        strcat(mes4, "\t");
        strcat(mes4, t2);
        strcat(mes4, " - Ya devolvi el recurso :)\n");
        strcat(txtContent, mes4);
    }
}

void* writeFile (char message[]) {
    fprintf(fp, "%s", message);
}

int main() {
    fp = fopen("SemaforoBitacora.txt", "w");

    printf("Iniciando programa.\n");
    strcat(txtContent, "Iniciando programa.\n");

    pthread_t threads[nthreads];
    void * retvals[nthreads];
    int i;

    sem_init(&mutex, 0, 2);

    printf("Creando threads.\n");
    strcat(txtContent, "Creando threads.\n");
    for (i = 0; i < nthreads; i++) {
        if (pthread_create(&threads[i], NULL, doNothing, NULL) != 0) {
            fprintf (stderr, "Error: No se puede crear thread # %d\n", i);
        } else {
            char s[5] = {0};
            char m[50] = {0};
            sprintf(s, "%d", i + 1);
            strcat(m, "Iniciando thread ");
            strcat(m, s);
            strcat(m, "\n");

            printf("Iniciando thread %d\n", i + 1);
            strcat(txtContent, m);
        }
    }

    printf("Esperando threads.\n");
    strcat(txtContent, "Esperando threads.\n");
    for (i = 0; i < nthreads; i++) {
        if (pthread_join(threads[i], &retvals[i]) != 0) {
            fprintf (stderr, "Error: No se puede hacer join al thread #%d\n", i);
        }
    }

    writeFile(txtContent);

    sem_destroy(&mutex);
    fclose(fp);
    return 0;
}