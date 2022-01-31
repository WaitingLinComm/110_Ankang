// Execute instruction: 
// (1) g++ -o d fft_ifft_vec.cpp -lfftw3 -lm
// (2) ./d
//
// NOTE:
// The code adopts fft/ifft implementation in FFTW library to perform the convolution of two complex vectors.
// To #include <fftw3.h>, one should follow instruction in https://www.fftw.org to install FFTW library.


#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <fftw3.h>
#include "ifft_vec.h"
#include "fft_vec.h"
#include "test_printf_2_vec.h"
using namespace std;

int main()
{   
    // Let X be the fft value of x
    vector<double>  X_I, X_Q, x_I, x_Q;
    int N = 4;

    // The input vector is [1+i1, 2+i2, 0, 0] in matlab language
    // NOTE: we need to reserve the vector space in advance to prevent the use of residual value
    X_I.reserve(N); X_Q.reserve(N);

    X_I.push_back(1); X_Q.push_back(1);
    
    X_I.push_back(2); X_Q.push_back(2);
   
    // do ifft
    ifft_vec(X_I, X_Q, x_I, x_Q, N);
    
    // do fft
    //fft_vec(X_I, X_Q, x_I, x_Q, N);
    
    // print the result 
    test_printf_2_vec(x_I, x_Q, N);

    return 0;
}