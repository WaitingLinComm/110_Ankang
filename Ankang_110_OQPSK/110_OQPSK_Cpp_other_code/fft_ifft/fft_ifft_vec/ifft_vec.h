#include <stdio.h>
#include <stdlib.h>
//#include <fftw3.h>
#include <vector>
using namespace std;

void ifft_vec(vector<double> &in_I, vector<double> &in_Q, vector<double>  &out_I, vector<double> &out_Q, int N)
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
    p = fftw_plan_dft_1d(N, in, out, FFTW_BACKWARD, FFTW_ESTIMATE); // FFTW_BACKWARD: for ifft
    
    fftw_execute(p); //repeat as needed //

    // ifft needs normalization, while fft don't need. 
    for (int i = 0; i < N; i++) {
    out[i][0] *= 1./N;
    out[i][1] *= 1./N;
    }       

    
    for(int i = 0; i < N; i++)
    {
        out_I.push_back(out[i][0]); //out_I[i] = out[i][0];
        out_Q.push_back(out[i][1]); //out_Q[i] = out[i][1];
    }
    

    fftw_destroy_plan(p);   
    fftw_free(in);
    fftw_free(out);
}