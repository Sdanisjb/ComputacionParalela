#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

int n_threads = 1;
int counter = 0;
pthread_mutex_t barrier_mutex;

/*Variables para semaforo*/
sem_t count_sem;
sem_t barrier_sem;

/*Variables para condition variable*/
pthread_mutex_t mutex;
pthread_cond_t cond_var;

void* Barrier_BW(void* rank){
    long my_rank = (long) rank;
    pthread_mutex_lock(&barrier_mutex);
    counter++;
    pthread_mutex_unlock(&barrier_mutex);
    while(counter < n_threads);

    if(my_rank==0){
        std::cout<<"Todos los threads llegaron a este punto"<<std::endl;
    }

    return NULL;
}

void* Barrier_Semaphore(void* rank){
    long my_rank = (long)rank;

    sem_wait(&count_sem);
    if(counter == n_threads-1){
        counter = 0;
        sem_post(&count_sem);
        for(int j=0;j<n_threads-1;j++){
            sem_post(&barrier_sem);
        }
    }
    else{
        counter++;
        sem_post(&count_sem);
        sem_wait(&barrier_sem);
    }

    if(my_rank == 0) std::cout << "Todos los threads llegaron a este punto" << std::endl;

    return NULL;
}

void* Barrier_CondVar(void* rank){
    long my_rank = (long)rank;

    pthread_mutex_lock(&mutex);
    counter++;
    if (counter == n_threads){
        counter = 0;
        pthread_cond_broadcast(&cond_var);
    }
    else{
        while(pthread_cond_wait(&cond_var, &mutex) != 0);
    }
    pthread_mutex_unlock(&mutex);
}


int main(int argc, char* argv[]){ 

    /*inicializar semaforas*/
    sem_init(&count_sem, 0, 1);
    sem_init(&barrier_sem, 0, 0);

    long i;

    n_threads = atoi(argv[1]);

    pthread_t* thread_handles = new pthread_t[n_threads];

    for(i =0;i<n_threads;i++){
        pthread_create(&thread_handles[i], NULL, Barrier_Semaphore, (void*)i); 
    }

    for(i=0;i<n_threads;i++){
        pthread_join(thread_handles[i], NULL);
    }


    delete[] thread_handles;    
}