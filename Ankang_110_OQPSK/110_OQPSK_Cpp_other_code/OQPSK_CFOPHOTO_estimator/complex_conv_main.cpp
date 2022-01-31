// Status: <complete> 
//
// The code 
// 1. read data from Sig_Re.txt, Sig_Im.txt, syncSig_Re.txt, and syncSig_Im.txt which contain the data from Ankang matlab code.
// 2. Use <fftw3.h> to perform complex fft.
// 
//      Example:
//      Suppose we would like to calculate z = conv(a, b), where a, b are complex value. 
//            
//      We can implement the convolution as the following way
//      t = fft(a, Lz) * fft(b, Lz), where Lz = La + Lb -1
//      z = ifft(t, Lz)
//
// Also, We can check the code running time by using
//      clock_t t1, t2;
//      t1 = clock();
//       ..... code .....
//      t2 = clock();
//      printf("%1f (sec)\n", (t2-t1) / (double)(CLOCKS_PER_SEC));
//

#include <iostream>
using namespace std;
#include <vector>
#include <fftw3.h>
#include <time.h>

#include "readtxt2array.h"
#include "norm2_vec.h"
#include "Flip_vec.h"
#include "test_printf_2_vec.h"
int main()
{
    clock_t t1, t2;
    //////////////////////     Read input file(from matlab)     ////////////////////// 
    int filelength = 532480;
    double *arraySig_I = (double*)malloc(filelength * sizeof(double)); 
    const char *filename1;
    filename1 = "Sig_Re.txt"; 
    readtxt2array(filename1, filelength, arraySig_I);

    filelength = 532480;
    double *arraySig_Q = (double*)malloc(filelength * sizeof(double)); 
    const char *filename2;
    filename2 = "Sig_Im.txt"; 
    readtxt2array(filename2, filelength, arraySig_Q);
    
    filelength = 532480;
    double *rfft_Re = (double*)malloc(filelength * sizeof(double)); 
    const char *filename3;
    filename3 = "rfft_Re.txt"; 
    readtxt2array(filename3, filelength, rfft_Re);

    filelength = 532480;
    double *rfft_Im = (double*)malloc(filelength * sizeof(double)); 
    const char *filename4;
    filename4 = "rfft_Im.txt"; 
    readtxt2array(filename4, filelength, rfft_Im);

    filelength = 858;
    double *syncSig_Re = (double*)malloc(filelength * sizeof(double)); 
    const char *filename5;
    filename5 = "syncSig_Re.txt"; 
    readtxt2array(filename5, filelength, syncSig_Re);

    filelength = 858;
    double *syncSig_Im = (double*)malloc(filelength * sizeof(double)); 
    const char *filename6;
    filename6 = "syncSig_Im.txt"; 
    readtxt2array(filename6, filelength, syncSig_Im);

    //////////////////////    Change array data type to vector   //////////////////////  
    int SigLength_afterResample = 532480;
    vector<double>  Sig_I(arraySig_I, arraySig_I + SigLength_afterResample); 
    vector<double>  Sig_Q(arraySig_Q, arraySig_Q + SigLength_afterResample); 

    vector<double>  rfft_I(rfft_Re, rfft_Re + SigLength_afterResample); 
    vector<double>  rfft_Q(rfft_Im, rfft_Im + SigLength_afterResample); 
    
    int L_syncSig = 858;
    vector<double>  syncSig_I(syncSig_Re, syncSig_Re + L_syncSig); 
    vector<double>  syncSig_Q(syncSig_Im, syncSig_Im + L_syncSig);    
    
    ////////////////////////////////////////////////////////////
    int L_q = L_syncSig;
    // (OK)realize : q(end : -1 : 1)
    vector<double>  q_flip_I, q_flip_Q;
    q_flip_I.reserve(L_q); q_flip_Q.reserve(L_q); 
    Flip_vec(q_flip_I, syncSig_I, L_q);
    Flip_vec(q_flip_Q, syncSig_Q, L_q);

    // (OK)realize : conj(q(end : -1 : 1)
    vector<double>  conj_q_flip_Q;
    conj_q_flip_Q.reserve(L_q); 
    for(int i = 0; i < L_q; i++){ 
        conj_q_flip_Q.push_back(- q_flip_Q[i]); // (X)conj_q_flip_Q[i] = - q_flip_Q[i];
    }

    ////////////////////////  realize q_fft = fft(conj(flip_q), Lz)   ////////////////////////
    int Lz;
    Lz = 533337; //L_r + L_q - 1; Do Lz points fft

    t1 = clock();
    fftw_complex *q, *q_fft;
    //int P = Lz; // P point fft
    q = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * Lz);
    q_fft = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * Lz);
    for(int i = 0; i < L_q; i++){
        q[i][0] = q_flip_I[i];
        q[i][1] = conj_q_flip_Q[i];
    }
    fftw_plan p2;
    p2 = fftw_plan_dft_1d(Lz, q, q_fft, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p2);
    fftw_destroy_plan(p2);
    fftw_free(q);   
    t2 = clock();
    printf("1st fft is done in %1f (sec)\n", (t2-t1) / (double)(CLOCKS_PER_SEC));
     
    ////////////////////////  realize r_fft = fft(r, Lz)   ////////////////////////
    t1 = clock();
    fftw_complex *r, *r_fft;
    r = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * Lz);
    r_fft = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * Lz);

    for(int i = 0; i < SigLength_afterResample; i++){
        r[i][0] = Sig_I[i];
        r[i][1] = Sig_Q[i];
    }
    fftw_plan p;
    p = fftw_plan_dft_1d(Lz, r, r_fft, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p);
    fftw_destroy_plan(p);
    fftw_free(r);
    t2 = clock();
    printf("2nd fft is done in %1f (sec)\n", (t2-t1) / (double)(CLOCKS_PER_SEC));
   
    ////////////////////////  realize z1_ifft = fft(r, Lz) * fft(conj(flip_q), Lz)   ////////////////////////
    t1 = clock();
    fftw_complex *z1, *z1_ifft_conv;
    z1 = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * Lz);
    z1_ifft_conv = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * Lz);
    
    for (int i = 0; i < Lz; i++){
        z1[i][0] = r_fft[i][0] * q_fft[i][0] - r_fft[i][1] * q_fft[i][1]; 
        z1[i][1] = r_fft[i][0] * q_fft[i][1] + r_fft[i][1] * q_fft[i][0]; 
    }
    fftw_free(r_fft);
    fftw_free(q_fft);

    fftw_plan p3;
    p3 = fftw_plan_dft_1d(Lz, z1, z1_ifft_conv, FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(p3); //repeat as needed 
    for (int i = 0; i < Lz; i++) {
    z1_ifft_conv[i][0] *= 1./Lz;
    z1_ifft_conv[i][1] *= 1./Lz;
    }
    fftw_destroy_plan(p3);
    fftw_free(z1);

    t2 = clock();
/*
    printf("ifft is done in %1f (sec)\n", (t2-t1) / (double)(CLOCKS_PER_SEC));
    for (int i = 0; i < 20; i++){
        printf("[%d]: %f + j*%f\n", i, z1_ifft_conv[i][0], z1_ifft_conv[i][1]);
    }
*/
    fftw_free(z1_ifft_conv);
    
/*    
    // transfer fftw_complex data type to vec data type
    vector<double>  C_r_fft_I; //(arraySig_I, arraySig_I + SigLength_afterResample); //, Sig_Q;
    vector<double>  C_r_fft_Q; //(arraySig_Q, arraySig_Q + SigLength_afterResample); //, Sig_Q;
    
    C_r_fft_I.reserve(SigLength_afterResample); C_r_fft_Q.reserve(SigLength_afterResample);
    for (int i = 0; i < P; i++){
        C_r_fft_I.push_back(r_fft[i][0]); C_r_fft_Q.push_back(r_fft[i][1]); 
    }
*/
    
    //////////    Calculate error    //////////   
    //norm2_vec(C_r_fft_I, C_r_fft_Q, rfft_I, rfft_Q, SigLength_afterResample);
/*
    for (int i = 0; i < 10; i++){
        printf("[%d]: %f + j*%f\n", i, r_fft[i][0], r_fft[i][1]);
    }
    for (int i = 0; i < 10; i++){
        printf("[%d]: %f + j*%f\n", i, rfft_I[i], rfft_Q[i]);
    }
*/
    return 0;
}