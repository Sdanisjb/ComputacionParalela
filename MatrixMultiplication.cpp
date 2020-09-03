#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <algorithm>

#define N 2000
#define bs 20

using namespace std;


void classicMatrixMultiplication(double** A, double** B, double** C, int x, int y , int z){
    double suma = 0.0;
    for(int i=0;i<x;i++){
        for(int j=0;j<y;j++){
            suma = 0.0;
            for(int k=0;k<z;k++){
                suma+= A[i][k]*B[k][j];
            }
            C[i][j] = suma;
        }
    }
}

void blockMatrixMultiplication(double** A, double** B, double** C, int n, int b){
    for(int i0=0;i0<n;i0+=b){
        for(int j0=0;j0<n;j0+=b){
            for(int k0=0;k0<n;k0+=b){
                for(int i=i0; i<min(i0+b,n);i++){
                    for(int j=j0;j<min(j0+b,n);j++){
                        for(int k=k0;k<min(k0+b,n);k++){
                            C[i][j] = C[i][j] + A[i][k]*B[k][j];
                        }
                    }
                }
            }
        }
    }

}


int main()
{
   double **A, **B, **C;

   /*Inicializar matrices*/
   A = new double*[N];
   B = new double*[N];
   C = new double*[N];


   for(int i=0;i<N;i++){
    A[i] = new double[N];
    B[i] = new double[N];
    C[i] = new double[N];
   }

   srand(time(NULL));

   for(int i=0;i<N;i++){
    for(int j=0;j<N;j++){
        A[i][j] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/100));
        B[i][j] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/100));
        C[i][j] = 0;
    }
   }

    auto start = std::chrono::system_clock::now();

    //classicMatrixMultiplication(A,B,C,N,N,N);
    blockMatrixMultiplication(A,B,C,N,bs);

    auto end = std::chrono::system_clock::now();


    std::chrono::duration<float, std::milli> duration = end -start;

    cout<<duration.count()<<endl;






   /*Liberar Memoria*/
   for(int i=0;i<N;i++){
        delete A[i];
        delete B[i];
        delete C[i];
   }

   delete A;
   delete B;
   delete C;

}
