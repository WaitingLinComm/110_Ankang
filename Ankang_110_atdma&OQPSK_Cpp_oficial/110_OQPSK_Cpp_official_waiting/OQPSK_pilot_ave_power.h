 #pragma once
#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include<bits/stdc++.h>
using namespace std;    

#include "OQPSK_DC_Blocker.h"                     // used in "OQPSK_main"
#include "OQPSK_DownConversion.h"                 // used in "OQPSK_main"
#include "OQPSK_complex_convolution.h"            // used in "OQPSK_main and "OQPSK_frontendTx.h"
#include "OQPSK_overlap_add.h"                    // used in "OQPSK_coarseTO_estimator.h"
#include "OQPSK_Filter_TransferFunction.h"        // used in "OQPSK_main" 
#include "OQPSK_sinc.h"                           // used in "OQPSK_resample.h"
#include "OQPSK_resample.h"                       // used in "OQPSK_main"
#include "OQPSK_SRRC.h"                           // used in "OQPSK_main"
#include "OQPSK_symbol_mapping.h"                 // used in "OQPSK_main"

void OQPSK_pilot_ave_power(vector<double> &rawDataI, vector<double> &rawDataQ, int IPOINT){

   ///////////////////////   Data Paremeters   ////////////////////// 
    //double fc = -338745;                    // center frequency of subcarrier
    // Tx info.
    //double fsy = 0.256 * pow(10, 6);        // symbol rate  
    double Tsy = 1/fsy;                     // symbol period
    // USRP info.
    //double fs_USRP = 6.25 * pow(10, 6);     // sampling frequency
    double fs_USRP = fs;
    double Ts_USRP = 1/fs_USRP;             // sampling period
    double sps_USRP = fs_USRP/fsy;          // SamplePerSymbol. NOTE: USRP_sps is not interger. While sps should be even integer, so Resample Process is needed later on. 
    
    ///////////////////////   Read File   //////////////////////////// 
    //const char *filename;
    //filename = "burst2_sr6.25_dtCplx_st8t"; //110 OQPSK
    //int filelength = 12.5 * pow(10, 6);   // Number of used samples: 1s data 
    //int filelength = 1.25 * pow(10, 6);   // Number of used samples: 0.1s data 
    //int filelength = 125 * pow(10,6);       // Number of used samples 
    int SigLength = filelength/2;    
    vector<double> rawDataI, rawDataQ;
    rawDataI.reserve(SigLength); rawDataQ.reserve(SigLength); 
    OQPSK_DataConversion(filename, filelength, rawDataI, rawDataQ);
    //printf("Data conversion is done in %1f(sec).\n", (t2-t1)/(double)(CLOCKS_PER_SEC));
    
    
    // ////////////////////////// DC Blocker (Remove DC offset)  /////////////////////// 
    // int filelength = pow(10, 6);  
    // int SigLength = filelength/2; 
    vector<double> Data_withoutDC_I, Data_withoutDC_Q;
    Data_withoutDC_I.reserve(SigLength); Data_withoutDC_Q.reserve(SigLength); 

    OQPSK_DC_Blocker(Data_withoutDC_I, Data_withoutDC_Q, SigLength, rawDataI, rawDataQ);
    rawDataI.clear(); rawDataQ.clear(); 
    //printf("DC offset of data is removed in %1f(sec).\n", (t4-t3)/(double)(CLOCKS_PER_SEC));

    //////////////////////////  Down Conversion  ///////////////////////////////////// 
    vector<double> DataDownconversion_I, DataDownconversion_Q;
    DataDownconversion_I.reserve(SigLength); DataDownconversion_Q.reserve(SigLength); 

    OQPSK_DownConversion(DataDownconversion_I, DataDownconversion_Q, Data_withoutDC_I, Data_withoutDC_Q, SigLength, fc, fs_USRP);
    Data_withoutDC_I.clear(); Data_withoutDC_Q.clear(); 
    //printf("Down conversion is done in %1f(sec).\n", (t6-t5)/(double)(CLOCKS_PER_SEC));

    /////////////////////////  Lowpass Filter  //////////////////////////////////////
    //  n = 3; Wn = 2/39;
    //  [b,a] = butter(n, Wn, 'low');
    //  b = [0.0004    0.0013    0.0013    0.0004];
    //  a = [1.0000   -2.6781    2.4060   -0.7243];
    //  Transfer function:
    //  H(z) = {b(1) + b(2)z^{-1} + b(3)z^{-2} + b(4)z^{-3}}/{a(1) + a(2)z^{-1} + a(3)z^{-2} + a(4)z^{-3}}
    //  data_LP = filter(b, a, data_demod);
    //  
    vector<double> data_LP_I, data_LP_Q;
    data_LP_I.reserve(SigLength); data_LP_Q.reserve(SigLength); 

    vector<double> b, a;
    b.reserve(4); a.reserve(4); 
    //  We get the value of a, b from matlab
    b.push_back(0.000447748392604308);
    b.push_back(0.00134324517781292);
    b.push_back(0.00134324517781292);
    b.push_back(0.000447748392604308);
    
    a.push_back(1);
    a.push_back(-2.67812669726477);
    a.push_back(2.4059972477312);
    a.push_back(-0.724288563325587);
 
    OQPSK_Filter_TransferFunction(b, a, DataDownconversion_I, DataDownconversion_Q, data_LP_I, data_LP_Q);
    b.clear(); a.clear(); 
    DataDownconversion_I.clear(); DataDownconversion_Q.clear(); 
 
    /////////////////////////  Resample(Sinc Interpolation)  /////////////////////////
    double old_sps = sps_USRP;
    double new_sps = 26;            // Set by ourselves
    double N = new_sps / old_sps;   // upsample by N. NOTE: N can be non-integer 
    double window = 1;              //double window = 6; // half-sided window length(used in SincInterpolation)  
    int SigLength_afterResample = floor(SigLength * N);
    vector<double> Sig_I, Sig_Q;
    Sig_I.reserve(SigLength_afterResample); Sig_Q.reserve(SigLength_afterResample); 
    OQPSK_resample(Sig_I, Sig_Q, SigLength_afterResample, data_LP_I, data_LP_Q, SigLength, N, window);
    //printf("Data resample is done in %1f(sec).\n", (t8-t7)/(double)(CLOCKS_PER_SEC));
    data_LP_I.clear(); data_LP_Q.clear();
    double new_fs = fs_USRP * N;  
    
    /////////////////////////  Pulse Shaping Filter  /////////////////////////
    int FilterSpanInSymbols = 6;
    float RolloffFactor = 0.3;
    int L_pulseshaping = FilterSpanInSymbols * new_sps + 1; //157
    vector<double> PulseshapingFilter;
    PulseshapingFilter.reserve(L_pulseshaping); 
    OQPSK_SRRC(PulseshapingFilter, FilterSpanInSymbols, RolloffFactor, new_sps, L_pulseshaping);
    
    // NOTE: Pulseshaping is a real value vector
    // Although Pulseshaping is a real value vector, we would like to set its imaginary part as 0
    // for the ease of later calculation of complex convolution.
    vector<double> Pulseshaping_I, Pulseshaping_Q;
    Pulseshaping_I.reserve(L_pulseshaping); Pulseshaping_Q.reserve(L_pulseshaping);
    for(int i = 0; i < L_pulseshaping; i++){
    Pulseshaping_I.push_back(PulseshapingFilter[i]);
    // set the imaginary part of Pulseshaping as zero
    Pulseshaping_Q.push_back(0);
    }

    /////////////////  Bit sequence generating parameters  /////////////////
    // if bitorder = 1: even number of b represents real bit value
    //                  odd number of b represents imaginary bit value
    // if bitorder = 2: even number of b represents imaginary bit value
    //                  odd number of b represents real bit value
    // if bitmap = 1: change bit value(1 / 0) to voltage (-1 / 1)
    // if bitmap = 2: change bit value(1 / 0) to voltage (1 / -1)
    //
    // if IQinterlace = 1: Q leads I
    // if IQinterlace = 2: I leads Q
    
    int bitorder = 1;
    int bitmap = 2;
    int IQinterlace = 1;
    ///////////////////// Get syncSig from pilots  ////////////////////
    /////////////////////    1. symbol_mapping     ////////////////////
    int Symbol_Length = 27;
    int Pattern_Length = Symbol_Length * 2;
    int pilotPattern[27] = {2, 1, 3, 0, 3, 1, 3, 3, 0, 1, 0, 3, 3, 1, 2, 0, 1, 0, 0, 1, 1, 3, 2, 3, 3, 1, 0}; // pilotPattern = '213031330103312010011323310'; 27 symbols => 54 bits in total (OQPSK pilot symbols form 中心)      
    vector<double> pilotPattern_vec(pilotPattern, pilotPattern + Symbol_Length);

    vector<double> m_p;
    m_p.reserve(Pattern_Length); //the memory pre-allocate for symbol mapping
    vector<double> pilotSym_I, pilotSym_Q;
    pilotSym_I.reserve(Symbol_Length); pilotSym_Q.reserve(Symbol_Length);
    vector<double> syncSig_I, syncSig_Q; 
    // NOTE! The memory of syncSig_I, syncSig_Q will be allocated in "OQPSK_frontendTx.h", because the length of syncSig_I and syncSig_Q are not know yet.
    // and we can get the length of syncSig_I and syncSig_Q after "OQPSK_frontendTx.h""
    OQPSK_symbol_mapping(m_p, pilotSym_I, pilotSym_Q, pilotPattern, Symbol_Length);   //Transfer the OQPSK pilot symbols to bits % 27*2 = 54 bits in total
    OQPSK_frontendTx(syncSig_I, syncSig_Q, m_p, PulseshapingFilter, L_pulseshaping, Pattern_Length, new_sps, bitorder, bitmap, IQinterlace);
    int L_syncSig = syncSig_I.size(); 
    //printf("OQPSK frontendTx process is done in %f(sec).\n", (t10-t9)/(double)(CLOCKS_PER_SEC)); 

    //////////////////////   Course TO estimation   //////////////////////  
    vector<double>  y1_I, y1_Q;
    vector<int> coarse_pilot_position_sample;
    y1_I.reserve(SigLength_afterResample); y1_Q.reserve(SigLength_afterResample);
    
    int down_sample = 5;
    vector<double>  Sig_sample_I, Sig_sample_Q;
    int L_Sig_sample = ceil(Sig_I.size()/down_sample);
    Sig_sample_I.reserve(L_Sig_sample); Sig_sample_Q.reserve(L_Sig_sample);
    for(int i = 0; i < L_Sig_sample; i++){ 
        Sig_sample_I.push_back(Sig_I[down_sample * i]); 
        Sig_sample_Q.push_back(Sig_Q[down_sample * i]);
    }

    vector<double>  syncSig_sample_I, syncSig_sample_Q;
    int L_syncSig_sample = ceil(syncSig_I.size()/down_sample);
    syncSig_sample_I.reserve(L_syncSig_sample); syncSig_sample_Q.reserve(L_syncSig_sample);
    for(int i = 0; i < L_syncSig_sample; i++){ 
        syncSig_sample_I.push_back(syncSig_I[down_sample * i]); 
        syncSig_sample_Q.push_back(syncSig_Q[down_sample * i]);
    }
    OQPSK_coarseTO_estimator(Sig_sample_I, Sig_sample_Q, L_Sig_sample, syncSig_sample_I, syncSig_sample_Q, L_syncSig_sample ,new_sps, y1_I, y1_Q, coarse_pilot_position_sample);

    vector<double>  q_flip_I, q_flip_Q;
    q_flip_I.reserve(L_q); q_flip_Q.reserve(L_q); 

    OQPSK_Flip_vec(q_flip_I, syncSig_sample_I, L_q);
    OQPSK_Flip_vec(q_flip_Q, syncSig_sample_Q, L_q);

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
    OQPSK_complex_convolution_with_norm(y1_norm2, L_r ,q_flip_I, conj_q_flip_Q, L_q, Sig_sample_I, Sig_sample_Q, L_r);

    double pilot_detect_power;


}  
    
    
