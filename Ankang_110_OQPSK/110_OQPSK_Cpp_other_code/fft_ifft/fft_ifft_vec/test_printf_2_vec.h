#include <iostream>
using namespace std;

void test_printf_2_vec(vector<double> &in_I, vector<double> &in_Q, int L)
{
    for (int i = 0; i < L; i++){
        printf("[%d]: %f + j*%f\n", i, in_I[i], in_Q[i]);
    }
}    