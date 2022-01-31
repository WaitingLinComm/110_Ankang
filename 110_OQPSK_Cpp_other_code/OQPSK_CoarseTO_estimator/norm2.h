#include <stdio.h>
#include <stdlib.h>

void norm2(double *arr1_I, double *arr1_Q, double *arr2_I, double *arr2_Q, int N)
{

    double sum_I = 0, sum_Q = 0, norm2 = 0;
    
    for(int i = 0; i < N; i++){
    sum_I = arr1_I[i] - arr2_I[i];
    sum_Q = arr1_Q[i] - arr2_Q[i];
    norm2 = norm2 + sum_I * sum_I + sum_Q * sum_Q;
    sum_I = 0, sum_Q = 0; 
    }
    printf("square norm  = %f", norm2);
}    