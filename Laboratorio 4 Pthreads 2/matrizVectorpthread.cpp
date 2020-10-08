#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>

using namespace std;

#define GET_TIME(now) { \
   struct timeval t; \
   gettimeofday(&t, NULL); \
   now = t.tv_sec + t.tv_usec/1000000.0; \
}


int *A, *x, * y;
int n_threads;
int m,n;

void *Pth_mat_vect(void* rank) {
   long my_rank = (long) rank;
   int i, j;
   int local_m = m/n_threads; 
   int my_first_row = my_rank*local_m;
   int my_last_row = my_first_row + local_m - 1;

   for (i = my_first_row; i <= my_last_row; i++) {
      y[i] = 0.0;
      for (j = 0; j < n; j++)
          y[i] += A[i*n+j]*x[j];
   }

   return NULL;
}  /* Pth_mat_vect */


int main(int argc, char* argv[]){
    n_threads = atoi(argv[1]);
    cin>>m>>n;

    A = new int[m*n];
    x = new int[n];
    y = new int[m];

    pthread_t* thread_handles = new pthread_t[n_threads];

    for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
            A[i*n + j] = rand()%1000;
            x[j] = rand()%1000;
        }
    }
    int start, end;
    GET_TIME(start);
    for(long thread=0;thread<n_threads;thread++){
        pthread_create(&thread_handles[thread],NULL,Pth_mat_vect, (void*)thread);
    }

    for(long i=0;i<n_threads;i++){
        pthread_join(thread_handles[i], NULL);
    }
    GET_TIME(end);
    cout<<start-end<<endl;

    delete[] thread_handles;

    delete[] A;
    delete[] x;
    delete[] y;

}




