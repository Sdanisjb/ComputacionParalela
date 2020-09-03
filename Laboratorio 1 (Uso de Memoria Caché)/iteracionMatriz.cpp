#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <chrono>

#define MAX 20000

using namespace std;


int main()
{
    double **A, *x, *y;
    /*Separar memoria para ejecutar los programas*/
    A = new double*[MAX];
    for(int i=0;i<MAX;i++){
        A[i] = new double[MAX];
    }

    x = new double[MAX];
    y = new double[MAX];


    /*Dar seed a la función rand()*/
    srand(time(NULL));


    /*Inicializar los valores de A y X, dejar y en 0*/
    for(int i=0;i<MAX;i++){
        for(int j=0;j<MAX;j++){
            A[i][j] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/100));
        }
        x[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/100));
    }




    /*Primer for para recorrer la matriz*/
    auto start = std::chrono::system_clock::now();

    for(int i=0;i<MAX;i++){
        for(int j=0;j<MAX;j++){
            y[i] = A[i][j]*x[j];
        }
    }

    auto end = std::chrono::system_clock::now();


    std::chrono::duration<float, std::milli> duration = end -start;

    cout<<duration.count()<<endl;


    start = std::chrono::system_clock::now();
    /*Segundo for para recorrer la matriz*/
    for(int j=0;j<MAX;j++){
        for(int i=0;i<MAX;i++){
            y[i] = A[i][j]*x[j];
        }
    }
    end = std::chrono::system_clock::now();

    duration = end -start;

    cout<<duration.count()<<endl;

    /*Liberar memoria*/
    for(int i=0;i<MAX;i++){
        delete A[i];
    }
    delete A;

    delete x;
    delete y;

}