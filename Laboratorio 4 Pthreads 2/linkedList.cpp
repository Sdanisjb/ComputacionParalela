#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "LLheader.h"

#define GET_TIME(now) { \
   struct timeval t; \
   gettimeofday(&t, NULL); \
   now = t.tv_sec + t.tv_usec/1000000.0; \
}



int main(int argc, char* argv[]){

    double start, end;
    n_threads = atoi(argv[1]);

    pthread_t* thread_handles;

    thread_handles = new pthread_t[n_threads];

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&head_mutex, NULL);

    /*Llenar lista enlazada*/
    int i, attempts, success, key;
    int inserts = 1000;
    unsigned int seed = 1;
    i = attempts = 0;

    while ( i < inserts && attempts < 2*inserts ) {
      key = rand_r(&seed) % 1000000;
      success = insert_mutex(key);
      attempts++;
      if (success) i++;
   }

    GET_TIME(start);
    for(long thread=0; thread<n_threads;thread++){
        pthread_create(&thread_handles[thread], NULL, thread_mutex_node, (void*)thread);
    }
    for(long thread=0; thread<n_threads;thread++){
        pthread_join(thread_handles[thread], NULL);
    }
    GET_TIME(end);



    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&head_mutex);

    std::cout << end - start <<std::endl;
    
    return 0;
}