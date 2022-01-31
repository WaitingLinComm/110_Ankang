#include <iostream>
using namespace std;

#include <time.h>
#include <vector>
#include <math.h>
#include "Flip_vec.h"
//#include "FlipVector.h"
//#include "find_high_pilot.h"
#include "find_high_pilot_v2.h"
//#include "test_printf_2.h"

//#include "test_printf_2_vec.h"
//#include "test_printf_1_vec.h"

//#include "arraytovec.h"
//#include "overlapadd.h"

#include "overlapadd_v2.h"

//#include "MatchFiltering_1.h"
//#include "fft_vec.h"

#include <fftw3.h>

//#include "fft_vec.h"
//#include "ifft_vec.h"

void coarseTO_estimator_test(vector<double> &r_I,  vector<double> &r_Q, int L_r, vector<double> &q_I, vector<double> &q_Q, int L_q, int sps, vector<double> &y1_I, vector<double> &y1_Q,  vector<int> &m)
{  
    clock_t t1, t2;
    t1 = clock();
    
    //cout << r_I.size() << "\n";
    // (OK)realize : q(end : -1 : 1)
    vector<double>  q_flip_I, q_flip_Q;
    q_flip_I.reserve(L_q); q_flip_Q.reserve(L_q); 
    Flip_vec(q_flip_I, q_I, L_q);
    Flip_vec(q_flip_Q, q_Q, L_q);
    //cout << q_flip_I.size() << "\n";

    // (OK)realize : conj(q(end : -1 : 1)
    vector<double>  conj_q_flip_Q;
    conj_q_flip_Q.reserve(L_q); 
    for(int i = 0; i < L_q; i++){ 
        conj_q_flip_Q.push_back(- q_flip_Q[i]); // (X)conj_q_flip_Q[i] = - q_flip_Q[i];
    }

    // (OK)realize : y1 = filter(conj(q(end : -1 : 1)), 1, r);
    int L = 950; // block size
    //int L_out = L_r + L_q - 1;   // realize conv(x,h)
    int L_out = L_r;             // realize filter(h,1,x)

    y1_I.reserve(L_out); y1_Q.reserve(L_out);

    //clock_t t1, t2;
    //t1 = clock();
    //overlapadd_v2(y1_I, y1_Q, L_out, r_I, r_Q, L_r, q_flip_I, conj_q_flip_Q, L_q, L); // faster
    //overlapadd(y1_I, y1_Q, L_out, r_I, r_Q, L_r, q_flip_I, conj_q_flip_Q, L_q, L); //slower
    //overlapadd(y1_I, y1_Q, r_I, r_Q, L_r, q_flip_I, conj_q_flip_Q, L_q, L); // old version
    printf("overlap add method for complex convolution is done!!!!!!!!!!\n");
    //t2 = clock();
    //printf("%1f (sec)\n", (t2-t1) / (double)(CLOCKS_PER_SEC));

    //test_printf_2_vec(r_I, r_Q, 20); 
    
    t2 = clock();
    printf("%1f (sec)\n", (t2-t1) / (double)(CLOCKS_PER_SEC));

    /////////////////     conv_fft_fft_ifft    //////////////////////
    int Lz;
    Lz = L_r + L_q - 1; //533337
    // realize h_fft_1 = fft(r, Lz);
    
    fftw_complex *r;
    fftw_complex *r_fft;
    int P = Lz; // P point fft
    r = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * P);
    r_fft = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * P);
    //h_fft_2 = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * P);

    t1 = clock();
    for(int i = 0; i < L_r; i++){
        r[i][0] = r_I[i];
        r[i][1] = r_Q[i];
    }
    t2 = clock();
    printf("%1f (sec)\n", (t2-t1) / (double)(CLOCKS_PER_SEC));

    fftw_plan p;
    t1 = clock();
    p = fftw_plan_dft_1d(P, r, r_fft, FFTW_FORWARD, FFTW_ESTIMATE);
    t2 = clock();
    printf("%1f (sec)\n", (t2-t1) / (double)(CLOCKS_PER_SEC));

    t1 = clock();
    fftw_execute(p);
    t2 = clock();
    printf("%1f (sec)\n", (t2-t1) / (double)(CLOCKS_PER_SEC));

    t1 = clock();
    fftw_destroy_plan(p);
    t2 = clock();
    printf("%1f (sec)\n", (t2-t1) / (double)(CLOCKS_PER_SEC));

    fftw_free(r);
    
    for (int i = 0; i < 10; i++){
        printf("[%d]: %f + j*%f\n", i, r_fft[i][0], r_fft[i][1]);
    }

/*
    // realize h_fft_2 = fft(y1, Lz);
    h = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * P);
    for(int i = 0; i < P; i++){
        h[i][0] = y1_I[i];
        h[i][1] = y1_Q[i];
    }
    //fftw_plan p;
    p = fftw_plan_dft_1d(P, h, h_fft_2, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p);
    fftw_destroy_plan(p);
    fftw_free(h);

    // realize h = h_fft_1 .* h_fft_2
    Y_ifft = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * P); 
    for (int i = 0; i < P; i++){
        Y_ifft[i][0] = h_fft_1[i][0] * h_fft_2[i][0] - h_fft_1[i][1] * h_fft_2[i][1]; // real part 
        Y_ifft[i][1] = h_fft_1[i][0] * h_fft_2[i][1] + h_fft_1[i][1] * h_fft_2[i][0]; // imag part      
    } 

    // realize ifft(h_fft_1 .* h_fft_2)
    Y = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * P);
    Y_ifft_I.reserve(P); Y_ifft_Q.reserve(P); // 等一下會轉回vec形式

    p = fftw_plan_dft_1d(P, Y_ifft, Y, FFTW_BACKWARD, FFTW_ESTIMATE); // FFTW_BACKWARD: for ifft

    fftw_execute(p); //repeat as needed //

    // ifft needs normalization, while fft don't need. 
    for (int i = 0; i < P; i++) {
    Y[i][0] *= 1./P; 
    Y[i][1] *= 1./P;
    // 轉回vec形式
    Y_ifft_I.push_back(Y[i][0]); //Y_ifft_I[i] = Y[i][0];
    Y_ifft_Q.push_back(Y[i][1]); //Y_ifft_Q[i] = Y[i][1];
    }    
    fftw_destroy_plan(p);   
    fftw_free(Y_ifft);
    fftw_free(Y);
*/

    /*
    for (int i = 0; i < 10; i++){
        printf("[%d]: %f + j*%f\n", i, h_fft_2[i][0], h_fft_2[i][1]);
    }
    */



/*   
    vector<double> y1_norm2;
    y1_norm2.reserve(L_r);
    for (int i = 0; i < L_r; i++){
        y1_norm2.push_back(pow(y1_I[i],2) + pow(y1_Q[i],2));
    }
    vector<int> n;  
    find_high_pilot_v2(n, y1_norm2, sps);
    //test_printf_1_vec(n, n.size());   

    int number_of_pilot = n.size();
    m.reserve(number_of_pilot);
    for (int i = 0; i < number_of_pilot; i++){
        m.push_back(n[i] - L_q + 1);
    }
    //test_printf_1_vec(m, m.size()); 
*/
}
