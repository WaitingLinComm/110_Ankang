//#include <stdio.h>
//#include <stdlib.h>
#include <iostream>
using namespace std;

void test_printf_1_vec(vector<double> &in_I, int L)
{
    for (int i = 0; i < 20 + L; i++){
        printf("[%d]: %f\n", i, in_I[i]);
    }

    int K = 27;//532480;
    printf("[%d]: %f\n", K-3, in_I[K - 3]);
    printf("[%d]: %f\n", K-2, in_I[K - 2]);
    printf("[%d]: %f\n", K-1, in_I[K - 1]);
    printf("[%d]: %f\n", K, in_I[K]);
}    