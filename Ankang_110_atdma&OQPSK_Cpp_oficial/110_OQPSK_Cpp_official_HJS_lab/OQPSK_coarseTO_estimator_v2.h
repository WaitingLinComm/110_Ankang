//////////////////////////////////////////////////////////////////
// coarseTO_estimator 
// 	[MODE 1][original]  use coarseTO_estimator.h
//                             - sol1: use complex convolution
//                             - sol2: use overlap add method  
// 	[MODE 2][faster]    use coarseTO_estimator_v2.h 
//                             - speed up version of coarseTO_estimator.h.
//                             - use complex_convolution_with_norm.h (merge complex_convolution.h with y1_norm2.h)
//  [MODE 3][fastest]
//             Before implement coarseTO_estimator, we found that we can downsample  signal(:) and pilot(:) by 5 before calculating the above convolution. 
//             And then multiply  the position we find  by 5 would become the coarse pilot position.
//             Although there are some errors in the found coarse pilot position, we can correct the errors by the later CFOPHOTO_estimator.
//////////////////////////////////////////////////////////////////
// #include <fftw3.h>
// #include <iostream>
// using namespace std;
// #include <vector>
// #include "complex_convolution.h"
// #include "find_high_pilot.h"

void OQPSK_coarseTO_estimator_v2(vector<double> &r_I,  vector<double> &r_Q, int L_r, vector<double> &q_I, vector<double> &q_Q, int L_q, int sps, vector<double> &y1_I, vector<double> &y1_Q,  vector<int> &start_burst_position_index, vector<int> &end_burst_position_index)
{  
    clock_t t1, t2;
    // realize : q(end : -1 : 1)
    vector<double>  q_flip_I, q_flip_Q;
    q_flip_I.reserve(L_q); q_flip_Q.reserve(L_q); 

    OQPSK_Flip_vec(q_flip_I, q_I, L_q);
    OQPSK_Flip_vec(q_flip_Q, q_Q, L_q);

    // realize : conj(q(end : -1 : 1)
    vector<double>  conj_q_flip_Q;
    conj_q_flip_Q.reserve(L_q); 
    for(int i = 0; i < L_q; i++){ 
        conj_q_flip_Q.push_back(- q_flip_Q[i]); 
    }
    
    // NOTE!
    // If we would like to perform z = filter(x, 1, y), the output length Lz = Ly.
    // If we would like to perform z = conv(x, y) the output length Lz = Lx + Ly - 1.
    //
    ////////////////       realize : y1 = filter(conj(q(end : -1 : 1)), 1, r)     /////////////////////
    // t1 = clock();
    // int L_out = L_r + L_q - 1;   // realize y1 = conv(conj(q(end : -1 : 1)), 1, r)
    int L_out = L_r;                // realize y1 = filter(conj(q(end : -1 : 1)), 1, r)
    y1_I.reserve(L_out); y1_Q.reserve(L_out);
    vector<double> y1_norm2;
    y1_norm2.reserve(L_r);
    OQPSK_complex_convolution_with_norm(y1_norm2, L_r ,q_flip_I, conj_q_flip_Q, L_q, r_I, r_Q, L_r);
    // t2 = clock();
    // printf("Complex convolution without overlap add method is computed in %1f (sec)\n", (t2-t1) / (double)(CLOCKS_PER_SEC)); 

    //t1 = clock();
    vector<int> start_burst; // pilot position 
    vector<int> end_burst;
    // sol1:
    //find_high_pilot(start_burst_position, y1_norm2, sps);
    // sol2:
    OQPSK_find_burst_interval(start_burst, end_burst, y1_norm2, sps);
    //t2 = clock();
    //printf("Course pilot position is computed in %1f (sec)\n", (t2-t1) / (double)(CLOCKS_PER_SEC));//0.007445(s)
    int number_of_pilot = start_burst.size();
    start_burst.reserve(number_of_pilot);
    for (int i = 0; i < number_of_pilot; i++){
        start_burst_position_index.push_back(start_burst[i] - L_q + 1);
        end_burst_position_index.push_back(end_burst[i] - L_q + 1);
    }
}