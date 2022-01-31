//#include <stdio.h>
//#include <stdlib.h>
#include <iostream>
using namespace std;

void test_printf_2_vec(vector<double> &in_I, vector<double> &in_Q, int L)
{
    for (int i = 0; i < L; i++){
        printf("[%d]: %f + j*%f\n", i, in_I[i], in_Q[i]);
    }

    int K = 54;//532480;
    printf("[%d]: %f + j*%f\n", K-3, in_I[K - 3], in_Q[K - 3]);
    printf("[%d]: %f + j*%f\n", K-2, in_I[K - 2], in_Q[K - 2]);
    printf("[%d]: %f + j*%f\n", K-1, in_I[K - 1], in_Q[K - 1]);
    printf("[%d]: %f + j*%f\n", K, in_I[K], in_Q[K]);
}    