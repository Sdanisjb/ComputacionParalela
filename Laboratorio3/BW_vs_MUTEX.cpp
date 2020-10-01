#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <iomanip>
#include <chrono>
#include <sys/time.h>

#define GET_TIME(now) { \
   struct timeval t; \
   gettimeofday(&t, NULL); \
   now = t.tv_sec + t.tv_usec/1000000.0; \
}


#define n 10000000000

int n_threads;
double sum = 0.0;
int flag = 0;
pthread_mutex_t mutex;

void* Thread_sumBW(void* rank) {
    long my_rank = (long)rank;
    double my_sum = 0.0;
    double factor;
    long long i;
    long long my_n = n / n_threads;
    long long my_first_i = my_n * my_rank;
    long long my_last_i = my_first_i + my_n;

    if (my_first_i % 2 == 0)
        factor = 1.0;
    else
        factor = -1.0;

    for (i = my_first_i; i < my_last_i; i++, factor = -factor) {
        my_sum += factor / (2 * i + 1);
    }

    while (flag != my_rank);
    sum += my_sum;
    flag = (flag + 1) % n_threads;
    return NULL;
}

void* Thread_sumMUTEX(void* rank) {
    long my_rank = (long)rank;
    double my_sum = 0.0;
    double factor;
    long long i;
    long long my_n = n / n_threads;
    long long my_first_i = my_n * my_rank;
    long long my_last_i = my_first_i + my_n;

    if (my_first_i % 2 == 0)
        factor = 1.0;
    else
        factor = -1.0;

    for (i = my_first_i; i < my_last_i; i++, factor = -factor) {
        my_sum += factor / (2 * i + 1);
    }

    pthread_mutex_lock(&mutex);
    sum += my_sum;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char* argv[])
{
    double start,end;
    n_threads = atoi(argv[1]);


    GET_TIME(start);

    pthread_t* thread_handles;

    thread_handles = (pthread_t*)malloc(n_threads * sizeof(pthread_t));

    pthread_mutex_init(&mutex, NULL);

    for (long thread = 0; thread < n_threads; thread++) {
        pthread_create(&thread_handles[thread], NULL, Thread_sumBW, (void*) thread);
    }

    for (int thread = 0; thread < n_threads; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }

    sum*=4.0;

    GET_TIME(end);

    std::cout<<end-start<<std::endl;
}
