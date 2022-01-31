// Do ift of a complex vector
// The following code implement ifft([1+i*1, 2+i*2], 4) as an example

// Execute instruction: 
// (1) g++ -o d ifft_hello_world.cpp.cpp -lfftw3 -lm
// (2) ./d
//
// NOTE:
// The code adopts fft/ifft implementation in FFTW library to perform the convolution of two complex vectors.
// To #include <fftw3.h>, one should follow instruction in https://www.fftw.org to install FFTW library.


#include <stdio.h>
#include <fftw3.h>

int main()

{
    printf("Hello, world!\n");
    int N = 4;

    int i;

    fftw_complex *in, *out;

    fftw_plan p;

    // ...
    in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);

    out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);

    in[0][0] = 1;
    in[0][1] = 1; // The input vector is [1+i1, 2+i2, 0, 0] in matlab language

    in[1][0] = 2;
    in[1][1] = 2;

    // forward Fourier transform
    //p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    // backward Fourier transform
    p = fftw_plan_dft_1d(N, in, out, FFTW_BACKWARD, FFTW_ESTIMATE);

    // ...
    fftw_execute(p); /*repeat as needed */

    // ifft needs normalization, while fft don't need. 
    for (int i = 0; i < N; i++) {
    out[i][0] *= 1./N;
    out[i][1] *= 1./N;
    }
       
    for (i = 0; i < N; i++)
    printf("[%d]: %f + j*%f\n", i, out[i][0], out[i][1]);

    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
    return 0;

}