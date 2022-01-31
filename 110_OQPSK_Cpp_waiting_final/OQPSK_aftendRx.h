// #include <vector>
// #include <iostream>
// using namespace std;
// #include "conv_vec.h"
void OQPSK_aftendRx(vector<double> &b_i_hat, vector<double> &b_q_hat, vector<double> &SigCFOCPOTOcompen_I, vector<double> &SigCFOCPOTOcompen_Q, int L_SigCFOCPOTOcompen, vector<double> &Pulseshaping_I, vector<double> &Pulseshaping_Q, int L_pulseshaping, int FilterSpanInSymbols, int sps)
{  
    // realize: Pulseshaping(end:-1:1)
    vector<double> Pulseshaping_flip_I, Pulseshaping_flip_Q;
    Pulseshaping_flip_I.reserve(L_pulseshaping); Pulseshaping_flip_Q.reserve(L_pulseshaping);
    
    for(int i = 0; i < L_pulseshaping; i++){
        Pulseshaping_flip_I.push_back(Pulseshaping_I[L_pulseshaping - 1 -i]);
        Pulseshaping_flip_Q.push_back(Pulseshaping_Q[L_pulseshaping - 1 -i]);
    }
    //Pulseshaping_I.clear(); Pulseshaping_Q.clear();

    // realize: u_hat = conv(Pulseshaping(end:-1:1), SigCFOCPOTOcompen) 
    vector<double> u_hat_I, u_hat_Q;
    int L_u_hat = L_pulseshaping + L_SigCFOCPOTOcompen - 1;
    u_hat_I.reserve(L_u_hat); u_hat_Q.reserve(L_u_hat);
    conv_vec(SigCFOCPOTOcompen_I, Pulseshaping_flip_I, u_hat_I, L_SigCFOCPOTOcompen, L_pulseshaping, L_u_hat);
    conv_vec(SigCFOCPOTOcompen_Q, Pulseshaping_flip_I, u_hat_Q, L_SigCFOCPOTOcompen, L_pulseshaping, L_u_hat);
    // SigCFOCPOTOcompen_I.clear(); SigCFOCPOTOcompen_Q.clear();

    // Down sample by sps/2
    // realize: l_hat = u_hat(1:sps/2:end)
    vector<double> l_hat_I, l_hat_Q;
    int L_l_hat = L_u_hat/(sps/2);
    l_hat_I.reserve(L_l_hat); l_hat_Q.reserve(L_l_hat);
    for(int i = 0; i < L_l_hat; i++){
        l_hat_I.push_back(u_hat_I[sps/2 * i]);
        l_hat_Q.push_back(u_hat_Q[sps/2 * i]);
    }
    
    vector<double> l_hat_truncate_I, l_hat_truncate_Q;
    int L_l_hat_truncate = L_l_hat - FilterSpanInSymbols * 2 - FilterSpanInSymbols * 2;
    l_hat_truncate_I.reserve(L_l_hat_truncate); l_hat_truncate_Q.reserve(L_l_hat_truncate);
    for(int i = FilterSpanInSymbols * 2 ; i < L_l_hat - FilterSpanInSymbols * 2; i++){
        l_hat_truncate_I.push_back(l_hat_I[i]);
        l_hat_truncate_Q.push_back(l_hat_Q[i]);
    }
    
    // Align I, Qch, ... and get b_i_hat, b_q_hat
    // IQinterlace == 1
    // Real part
    // vector<double> b_i_hat, b_q_hat;
    int L_b_hat = L_l_hat_truncate/2;
    b_i_hat.reserve(L_b_hat); b_q_hat.reserve(L_b_hat);

    for(int i =  0; i < L_b_hat; i++){
        b_i_hat.push_back(l_hat_truncate_I[2 * i]);
        b_q_hat.push_back(l_hat_truncate_Q[2 * i + 1]);
    }    
    // Get the received symbol
    vector<double> RxSym;
    int L_RxSym = L_l_hat;
    RxSym.reserve(L_RxSym);
    double max = -10000;
    for(int i =  0; i < L_RxSym; i++){
        RxSym.push_back(b_i_hat[i]);
        RxSym.push_back(b_q_hat[i]);
        if(b_i_hat[i] > max)
            max = b_i_hat[i];
        if(b_q_hat[i] > max) 
            max = b_q_hat[i];
    }    
    // Normalize the received symbol
    // realize: RxSym_normalized = RxSym/max(RxSym)
    vector<double> RxSym_normalized;
    RxSym_normalized.reserve(L_RxSym);
    for(int i =  0; i < L_RxSym; i++){
        RxSym_normalized.push_back(RxSym[i]/max);
    }  
}        