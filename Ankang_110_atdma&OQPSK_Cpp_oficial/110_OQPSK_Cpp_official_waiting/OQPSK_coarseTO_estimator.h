//#include "Flip_vec.h"
//#include "find_high_pilot.h"
//#include "find_high_pilot.h" // use complex_convolution_with_norm.h (merge "complex_convolution.h" with "y1_norm2.h")

void OQPSK_coarseTO_estimator(vector<double> &r_I,  vector<double> &r_Q, int L_r, vector<double> &q_I, vector<double> &q_Q, int L_q, int sps, vector<double> &y1_I, vector<double> &y1_Q,  vector<int> &m)
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
        conj_q_flip_Q.push_back(- q_flip_Q[i]); // (X)conj_q_flip_Q[i] = - q_flip_Q[i];
    }
    
    // NOTE!
    // If we would like to perform z = filter(x, 1, y), the output length Lz = Ly.
    // If we would like to perform z = conv(x, y) the output length Lz = Lx + Ly - 1.
    //
    ////////////////       realize : y1 = filter(conj(q(end : -1 : 1)), 1, r)     /////////////////////
    //int L_out = L_r + L_q - 1;   // realize y1 = conv(conj(q(end : -1 : 1)), 1, r)
    int L_out = L_r;               // realize y1 = filter(conj(q(end : -1 : 1)), 1, r)
    y1_I.reserve(L_out); y1_Q.reserve(L_out);

    vector<double> y1_norm2;
    y1_norm2.reserve(L_r);
    OQPSK_complex_convolution_with_norm(y1_norm2, L_r ,q_flip_I, conj_q_flip_Q, L_q, r_I, r_Q, L_r);
            
    vector<int> n;  
    OQPSK_find_high_pilot(n, y1_norm2, sps);

    int number_of_pilot = n.size();
    m.reserve(number_of_pilot);
    for (int i = 0; i < number_of_pilot; i++){
        m.push_back(n[i] - L_q + 1);
    }
}