// Overlapadd.h
// The code realize Overlap add method
/////////////////////////////////////////////////////////
// The C++ code implement the folowing matlab code
/////////////////////////////////////////////////////////
/*
    function [X] = overlapadd(x, h, L)

    N1 = length(x);
    M = length(h);
    lc = conv(x,h);
    x = [x zeros(1, mod(N1, L))];
    N2 = length(x);
    h = [h zeros(1, L - 1)];
    H = fft(h, L + M - 1);
    S = N2/L;
    index = 1:L;
    X = [zeros(M-1)];
    for stage=1:S
        xm = [x(index) zeros(1, M - 1)];                    % Selecting sequence to process
        X1 = fft(xm, L + M - 1);
        Y = X1 .* H;
        Y = ifft(Y);
        Z = X((length(X) - M + 2):length(X)) + Y(1:M-1);	% Samples Added in every stage
        X = [X(1:(stage - 1) * L) Z Y(M:M+L-1)];
        index=stage*L+1:(stage+1)*L;
    end
    i=1:N1+M-1;
    X = X(i);
*/
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
#include <fftw3.h>
void overlapadd_v2(vector<double> &X_out_I, vector<double> &X_out_Q, int L_out, vector<double> &x_I, vector<double> &x_Q, int N1, vector<double>  &h_I, vector<double> &h_Q, int M, int L)
{
    // realize: x = [x zeros(1, mod(N1, L))];
    int zero_padding = N1 % L; 
    for(int i = 1; i <= zero_padding; i++){
        x_I.push_back(0); x_Q.push_back(0);
    }
    int N2 =  N1 + zero_padding; 

    // realize: h = [h zeros(1, L - 1)];
    for(int i = 1; i <= L - 1; i++){ 
        h_I.push_back(0); h_Q.push_back(0);
    }

    //******************     fft      *******************
    // realize H = fft(h, L + M - 1);
    fftw_complex *h, *h_fft;
    int P = L + M - 1; // P point fft
    h = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * P);
    h_fft = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * P);

    for(int i = 0; i < P; i++){
        h[i][0] = h_I[i];
        h[i][1] = h_Q[i];
    }
    fftw_plan p;
    p = fftw_plan_dft_1d(P, h, h_fft, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p);
    fftw_destroy_plan(p);
    fftw_free(h);
    //for (int i = 0; i < P; i++){
    //    printf("[%d]: %f + j*%f\n", i, h_fft[i][0], h_fft[i][1]);
    //}
    ////////////////////////////////////////////////
    int S = N2/L;
    vector<double>  X_I, X_Q;
    int L_x = M + L - 1; // NOTE: L_x is different in each loop

    X_I.reserve(M + L - 1); X_Q.reserve(M + L - 1);
    for (int i = 1; i <= M + L - 1; i++){
        X_I.push_back(0); X_Q.push_back(0);
    }
    
    ////////////////////////     Iteration      ////////////////////////  
    // realize: index = 1 : L
    int index_start = 1 - 1;
    int index_end = L - 1;

    vector<double>  xm_I, xm_Q; 
    vector<double>  Y_ifft_I, Y_ifft_Q;
    vector<double>  Z_I, Z_Q;
    vector<double>  X_new_I, X_new_Q;

    for (int stage = 1; stage <= S; stage++){        
        // realize: xm = [x(index) zeros(1, M - 1)];        
        xm_I.clear(); xm_Q.clear();
        xm_I.reserve(L + M -1); xm_Q.reserve(L + M -1);
        for (int i = index_start; i <= index_end; i++){        
            xm_I.push_back(x_I[i]); xm_Q.push_back(x_Q[i]);
        }
        for (int i = 1; i <= M - 1; i++){        
            xm_I.push_back(0); xm_Q.push_back(0);            
        }
        ////////////////////     fft      ////////////////////
        // realize: X1 = fft(xm, L + M - 1); 
        fftw_complex *xm, *x1_fft;
        fftw_plan p;

        xm = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * P);
        x1_fft = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * P);

        for(int i = 0; i < P; i++){
            xm[i][0] = xm_I[i];
            xm[i][1] = xm_Q[i];
        }
        p = fftw_plan_dft_1d(P, xm, x1_fft, FFTW_FORWARD, FFTW_ESTIMATE);
        fftw_execute(p); //repeat as needed //
        fftw_destroy_plan(p);
        fftw_free(xm);
        //fftw_free(x1_fft); 
        /////////////////////////////////////////////
        // realize: Y = X1 .* H;   
        fftw_complex *Y, *Y_ifft;
        Y = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * P);
        Y_ifft = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * P);
        for (int i = 0; i < P; i++){        
            Y_ifft[i][0] = x1_fft[i][0] * h_fft[i][0] - x1_fft[i][1] * h_fft[i][1];
            Y_ifft[i][1] = x1_fft[i][0] * h_fft[i][1] + x1_fft[i][1] * h_fft[i][0];
        }
        fftw_free(x1_fft); //fftw_free(h_fft); 

        ////////////////////     ifft      ////////////////////
        // realize: Y = ifft(Y);; 
        Y_ifft_I.reserve(P); Y_ifft_Q.reserve(P); // 等一下會轉回vec形式

        p = fftw_plan_dft_1d(P, Y_ifft, Y, FFTW_BACKWARD, FFTW_ESTIMATE); // FFTW_BACKWARD: for ifft
    
        fftw_execute(p); //repeat as needed //

        // ifft needs normalization, while fft don't need. 
        for (int i = 0; i < P; i++) {
        Y[i][0] *= 1./P; 
        Y[i][1] *= 1./P;
        // 轉回vec形式
        Y_ifft_I[i] = Y[i][0];
        Y_ifft_Q[i] = Y[i][1];
        }    
        fftw_destroy_plan(p);   
        fftw_free(Y_ifft);
        fftw_free(Y);
        ///////////////////////////////////////////////////////////////////////
        // realize: Z = X((length(X) - M + 2):length(X)) + Y(1:M-1);    
        Z_I.reserve(M - 1); Z_Q.reserve(M - 1);

        for (int i = 0; i < M-1; i++){  
            Z_I.push_back(X_I[L_x - M + 2 - 1  + i] + Y_ifft_I[i]);
            Z_Q.push_back(X_Q[L_x - M + 2 - 1  + i] + Y_ifft_Q[i]);      
        }

        // realize: X_new = [X(1:(stage - 1) * L) Z Y(M:M+L-1)];
        L_x = (stage - 1) * L + (M - 1) + L;
        
        X_new_I.reserve(L_x); X_new_Q.reserve(L_x);
        //X_I.reserve(L_x); X_Q.reserve(L_x);
    
        //sol2:(faster)
        X_new_I.assign(X_I.begin(), X_I.begin() + (stage - 1) * L);
        X_new_Q.assign(X_Q.begin(), X_Q.begin() + (stage - 1) * L);

        for (int i = 0; i < M - 1; i++){       
            X_new_I.push_back(Z_I[i]); X_new_Q.push_back(Z_Q[i]); 
        }
        for (int i = M - 1; i < M + L; i++){           
            X_new_I.push_back(Y_ifft_I[i]); X_new_Q.push_back(Y_ifft_Q[i]);  
        }

        X_I.clear(); X_Q.clear();
        
        //X for next loop
        X_I.reserve(L_x); X_Q.reserve(L_x);
        X_I.assign(X_new_I.begin(), X_new_I.end()); //複製 X_new_I 到 X_I
        X_Q.assign(X_new_Q.begin(), X_new_Q.end()); //複製 v1 到 v2

        X_new_I.clear(); X_new_Q.clear();
        
        Z_I.clear(); Z_Q.clear();
        Y_ifft_I.clear(); Y_ifft_Q.clear();

        index_start = stage * L + 1 - 1;
        index_end = (stage + 1) * L - 1;        
    }

    // convolution output
    //vector<double>  X_out_I, X_out_Q;
    //int L_out = N1 + M - 1; // realize conv(x,h)
    //int L_out = N1; //realize filter(h,1,x)

    //X_out_I.reserve(L_out); X_out_Q.reserve(L_out);
    for (int i = 0; i < L_out; i++){           
        X_out_I.push_back(X_I[i]);  X_out_Q.push_back(X_Q[i]);  
    }
    //test_printf_2_vec(X_out_I, X_out_Q, L_out);
   
    fftw_free(h_fft);  
}

