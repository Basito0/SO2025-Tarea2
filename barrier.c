#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int threads = 5;
int stages = 4;


typedef struct Barrier {
    int count;
    int etapa;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Barrier;

Barrier barrera;

void barrierInit(){
    pthread_mutex_init(&barrera.mutex, NULL);
    pthread_cond_init(&barrera.cond, NULL);
    barrera.count = 0;
    barrera.etapa = 0;
}

void wait(){
    pthread_mutex_lock(&barrera.mutex);
    barrera.count++;

    if(barrera.count == threads){
        barrera.etapa++;
        barrera.count = 0;
        pthread_cond_broadcast(&barrera.cond);
    }
    else{
        pthread_cond_wait(&barrera.cond, &barrera.mutex);
    }

    pthread_mutex_unlock(&barrera.mutex);
}

void* execute(void* arg){
    int id = *((int*)arg);
    while(1){
        pthread_mutex_lock(&barrera.mutex);
        if (barrera.etapa >= stages) {
            pthread_mutex_unlock(&barrera.mutex);
            break;
        }
        int numero_aleatorio = rand() % 500000;
        usleep(100000 + numero_aleatorio);
        printf("%d Esperando en etapa %d\n", id, barrera.etapa);
        pthread_mutex_unlock(&barrera.mutex);
        wait();
        printf("%d pasó barrera en etapa %d\n", id, barrera.etapa-1);
    }
    
    return NULL;
}

int main(int argc, char* argv[]){

    /*DEBUG ARGUMENTOS
    if(argc == 2){
        threads = atoi(argv[1]);
        printf("threads set to: %d\n", threads);
    }
    if(argc == 3){
        threads = atoi(argv[1]);
        stages = atoi(argv[2]);
        printf("threads set to: %d\n", threads);
        printf("stages set to: %d\n", stages);
    }
    */
    
    pthread_t threads_list[threads];
    int ids[threads];
    barrierInit(&barrera);

    for(int i=0; i < threads; i++){
        ids[i] = i;
        if (pthread_create(&threads_list[i], NULL, execute, &ids[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }
    
    for(int i=0; i < threads; i++){
        pthread_join(threads_list[i], NULL);
    }
}