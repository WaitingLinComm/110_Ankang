#include <stdio.h>
#include <stdlib.h>
#include <fftw3.h>
using namespace std;

void fft_vec(vector<double> &in_I, vector<double> &in_Q, vector<double>  &out_I, vector<double> &out_Q, int N)
{
    fftw_complex *in, *out;
    fftw_plan p;

    in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);
    out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);

    for(int i = 0; i < N; i++)
    {
        in[i][0] = in_I[i];
        in[i][1] = in_Q[i];
    }
    p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    
    fftw_execute(p); //repeat as needed //
    
    //printf("%f %f", in[0][0], in[0][1]);

    for(int i = 0; i < N; i++)
    {
        out_I.push_back(out[i][0]); //out_I[i] = out[i][0];
        out_Q.push_back(out[i][1]); //out_Q[i] = out[i][1];
    }

    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);   
}