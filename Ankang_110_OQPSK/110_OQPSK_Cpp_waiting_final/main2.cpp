// 110 Ankang OQPSK demod
// Execute instruction: 
// (1) g++ -o d main2.cpp -lfftw3 -lm
// (2) ./d
//
// NOTE:
// The code adopts fft/ifft implementation in FFTW library to perform the convolution of two complex vectors.
// Recall: conv(x,y) = ifft(fft(x).*fft(y)) where x and y are two complex vectors.
// To #include <fftw3.h>, one should follow instruction in https://www.fftw.org to install FFTW library.

///////////////////////   MODE setting   //////////////////////////////////////////// 
// Please view Readme for MODE setting
// #define __DEBUG_MAIN__ // print some values for debugging 

// Set the mode of coarseTO_estimator(please refer to readme.txt to know the difference of each mode) 
#define __MODE_1__    // [original version] only demod the first K symbols in each burst
//#define __MODE_2__    // [faster version] only demod the first K symbols in each burst
//#define __MODE_3__    // [fastest version] only demod the first K symbols in each burst
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//#define __MODE_4__    // [original version] able to find the start & end position in each burst by using find_burst_interval.h and demod them
//#define __MODE_5__    // [faster version of __MODE_4__] able to find the start & end position in each burst by using find_burst_interval.h and demod them
// NOTE: the end position of burst cannot be found correctly in __MODE_5__
//#define __MODE_6__ 

// Set the mode of CFOPHOTO_estimator(please refer to readme.txt to know the difference of each mode) 
#define __MODE_A__ // can be used with __MODE_1__, __MODE_2__, __MODE_3__, __MODE_4__, __MODE_5__, __MODE_6__       
//#define __MODE_B__ // can be used with __MODE_1__, __MODE_2__, __MODE_3__
//#define __MODE_C__ // can be used with __MODE_1__, __MODE_2__, __MODE_3__
//#define __MODE_D__ // can be used with __MODE_4__, __MODE_5__ , __MODE_6__   

///////////////////////   Header files   ////////////////////////////////////////////  
#include <fftw3.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <iostream>
using namespace std;
#include <string> 
#include <vector>
#include <time.h>
#include <math.h>
#define pi 3.141592653589793238462643383279502884

#include "DataConversion.h"                 // used in main
#include "DC_Blocker.h"                     // used in main
#include "DownConversion.h"                 // used in main
#include <fstream>                          // used in "readtxt2array.h"
#include "readtxt2array.h"                  // used in main
#include "complex_convolution.h"            // used in main, "coarseTO_estimator", "find_burst_interval.h" and "OQPSK_frontendTx.h"
#include "overlap_add.h"                    // used in "coarseTO_estimator.h"
#include "Filter_TransferFunction.h"        // used in main 
#include "sinc.h"                           // used in "resample.h"
#include "resample.h"                       // used in main
#include "SRRC.h"                           // used in main
#include "symbol_mapping.h"                 // used in main

#include "bitReorganization.h"              // used in "OQPSK_frontendTx.h"
#include "bitMapping.h"                     // used in "OQPSK_frontendTx.h"
#include "I_Q_interlace.h"                  // used in "OQPSK_frontendTx.h"
#include "Upsample.h"                       // used in "OQPSK_frontendTx.h"
#include "OQPSK_frontendTx.h"               // used in main

#include "Flip_vec.h"                       // used in "coarseTO_estimator.h"
#include "sum_vec.h"                        // used in "find_high_pilot.h"
#include "MAX_vec.h"                        // used in "find_high_pilot.h"
#include "find_high_pilot.h"                // used in "coarseTO_estimator.h"
#include "complex_convolution_with_norm.h"  // used in "coarseTO_estimator_v2.h"
#include "find_burst_interval.h"            // used in "coarseTO_estimator_v3.h"
#include "coarseTO_estimator.h"             // used in main
#include "coarseTO_estimator_v2.h"          // used in main
#include "coarseTO_estimator_v3.h"          // used in main

#include "tmp_cfo_compen.h"                 // used in "CFOPHOTO_estimator.h"
#include "CFOPHOTO_estimator.h"             // used in main
#include "CFOPHOTO_estimator_v2.h"          // used in main
#include "CFOPHOTO_estimator_v3.h"          // used in main
#include "CFOPHOTO_estimator_v4.h"          // used in main

#include "get_databurst.h"                  // used in main
#include "CFOPHOcompen.h"                   // used in main
#include "conv_vec.h"                       // used in "OQPSK_aftendRx.h" 
#include "OQPSK_aftendRx.h"                 // used in main
#include "DecisionProcess.h"                // used in main
#include "bitReorganizationRx.h"            // used in main
#include "symbol_demapping.h"               // used in main
#include "SNRestimation.h"                  // used in main
#include "ErrorRateCalculation.h"           // used in main

// Other
// #include "test_printf_1_vec.h"
// #include "test_printf_2_vec.h"
// #include "norm.h"
int main() 
{ 
    clock_t t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16, t17, t18, t19, t20;
    clock_t tIteration_1, tIteration_2;
    clock_t tIteration_start, tIteration_end;
    clock_t total_start, total_end;

    string DataType("OQPSK"); 
    cout << "DataType: " << DataType << endl; 
    ///////////////////////   Data Paremeters   ////////////////////// 
    double fc = -338745; // center frequency of subcarrier
    // Tx info.
    double fsy = 0.256 * pow(10, 6);        // symbol rate  
    double Tsy = 1/fsy;                     // symbol period
    // USRP info.
    double fs_USRP = 6.25 * pow(10, 6);     // sampling frequency
    double Ts_USRP = 1/fs_USRP;             // sampling period
    double sps_USRP = fs_USRP/fsy;          // SamplePerSymbol. NOTE: USRP_sps is not interger. While sps should be even integer, so Resample Process is needed later on. 
    
    ///////////////////////   Read File   //////////////////////////// 
    const char *filename;
    filename = "burst2_sr6.25_dtCplx_st8t"; //110 OQPSK
    //int filelength = 12.5 * pow(10, 6);   // Number of used samples: 1s data 
    //int filelength = 1.25 * pow(10, 6);  // Number of used samples: 0.1s data 
    int filelength = pow(10, 6);         // Number of used samples 
    //int filelength = pow(10, 8);  
    //int filelength =  3.75 * pow(10, 8); // total samples
    //int filelength = 1.25*pow(10, 8);  

    int SigLength = filelength/2;    
    vector<double> rawDataI, rawDataQ;
    rawDataI.reserve(SigLength); rawDataQ.reserve(SigLength); 
    t1 = clock();
    DataConversion(filename, filelength, rawDataI, rawDataQ);
    t2 = clock();
    //printf("Data conversion is done in %1f(sec).\n", (t2-t1)/(double)(CLOCKS_PER_SEC));

    ///////////////////////// / DC Blocker (Remove DC offset)  /////////////////////// 
    total_start = clock(); //從此處開始計算總執行時間(DataConversion不需要算進時間)
    vector<double> Data_withoutDC_I, Data_withoutDC_Q;
    Data_withoutDC_I.reserve(SigLength); Data_withoutDC_Q.reserve(SigLength); 

    t3 = clock();
    DC_Blocker(Data_withoutDC_I, Data_withoutDC_Q, SigLength, rawDataI, rawDataQ);
    t4 = clock();
    rawDataI.clear(); rawDataQ.clear(); 
    //printf("DC offset of data is removed in %1f(sec).\n", (t4-t3)/(double)(CLOCKS_PER_SEC));

    //////////////////////////  Down Conversion  ///////////////////////////////////// 
    vector<double> DataDownconversion_I, DataDownconversion_Q;
    DataDownconversion_I.reserve(SigLength); DataDownconversion_Q.reserve(SigLength); 

    t5 = clock();
    DownConversion(DataDownconversion_I, DataDownconversion_Q, Data_withoutDC_I, Data_withoutDC_Q, SigLength, fc, fs_USRP);
    t6 = clock();
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

    Filter_TransferFunction(b, a, DataDownconversion_I, DataDownconversion_Q, data_LP_I, data_LP_Q);
    b.clear(); a.clear(); 
    DataDownconversion_I.clear(); DataDownconversion_Q.clear(); 
   
    /////////////////////////  Resample(Sinc Interpolation)  /////////////////////////
    double old_sps = sps_USRP;
    double new_sps = 26;            // Set by ourselves
    double N = new_sps / old_sps;   // upsample by N. NOTE: N can be non-integer 
    double window = 1; 
    //double window = 6;              // half-sided window length(used in SincInterpolation)  
    int SigLength_afterResample = floor(SigLength * N);
    vector<double> Sig_I, Sig_Q;
    Sig_I.reserve(SigLength_afterResample); Sig_Q.reserve(SigLength_afterResample); 
    t7 = clock();
    resample(Sig_I, Sig_Q, SigLength_afterResample, data_LP_I, data_LP_Q, SigLength, N, window);
    t8 = clock();
    //printf("Data resample is done in %1f(sec).\n", (t8-t7)/(double)(CLOCKS_PER_SEC));
    data_LP_I.clear(); data_LP_Q.clear();
    double new_fs = fs_USRP * N;  
    
    /////////////////////////  Pulse Shaping Filter  /////////////////////////
    int FilterSpanInSymbols = 6;
    float RolloffFactor = 0.3; 
    int L_pulseshaping = FilterSpanInSymbols * new_sps + 1; //157
	vector<double> PulseshapingFilter;
    PulseshapingFilter.reserve(L_pulseshaping); 
    SRRC(PulseshapingFilter, FilterSpanInSymbols, RolloffFactor, new_sps, L_pulseshaping);
    
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
    t9 = clock();

    symbol_mapping(m_p, pilotSym_I, pilotSym_Q, pilotPattern, Symbol_Length);   //Transfer the OQPSK pilot symbols to bits % 27*2 = 54 bits in total
    OQPSK_frontendTx(syncSig_I, syncSig_Q, m_p, PulseshapingFilter, L_pulseshaping, Pattern_Length, new_sps, bitorder, bitmap, IQinterlace);
    t10 = clock();
    int L_syncSig = syncSig_I.size(); 
    //printf("OQPSK frontendTx process is done in %f(sec).\n", (t10-t9)/(double)(CLOCKS_PER_SEC)); 
 
    //////////////////////   Course TO estimation   //////////////////////  
#ifdef __MODE_1__  // [MODE 1][original]
    vector<double>  y1_I, y1_Q;
    vector<int> coarse_pilot_position; // start_burst_position or pilot_position
    y1_I.reserve(SigLength_afterResample); y1_Q.reserve(SigLength_afterResample);
    t11 = clock(); 
    coarseTO_estimator(Sig_I, Sig_Q, SigLength_afterResample, syncSig_I, syncSig_Q, L_syncSig ,new_sps, y1_I, y1_Q, coarse_pilot_position); 
    int number_of_bursts;
    number_of_bursts = coarse_pilot_position.size();
    t12 = clock();       
    // printf("Course TO estimation is done in %1f(sec).\n", (t12-t11)/(double)(CLOCKS_PER_SEC));
#endif  
#ifdef __MODE_2__  // [MODE 2][faster] 
    vector<double>  y1_I, y1_Q;
    vector<int> coarse_pilot_position;
    y1_I.reserve(SigLength_afterResample); y1_Q.reserve(SigLength_afterResample);
    t11 = clock(); 
    coarseTO_estimator_v2(Sig_I, Sig_Q, SigLength_afterResample, syncSig_I, syncSig_Q, L_syncSig ,new_sps, y1_I, y1_Q, coarse_pilot_position);
    int number_of_bursts;
    number_of_bursts = coarse_pilot_position.size();
    t12 = clock();       
    // printf("Course TO estimation is done in %1f(sec).\n", (t12-t11)/(double)(CLOCKS_PER_SEC));
#endif     
#ifdef __MODE_3__  // [MODE 3][fastest]
    vector<double>  y1_I, y1_Q;
    t11 = clock(); 
    int down_sample = 5;
    vector<double>  Sig_sample_I, Sig_sample_Q;
    int L_Sig_sample = ceil(Sig_I.size()/down_sample);
    Sig_sample_I.reserve(L_Sig_sample); Sig_sample_Q.reserve(L_Sig_sample);
    for(int i = 0; i < L_Sig_sample; i++){ 
        Sig_sample_I.push_back(Sig_I[down_sample * i]); // (X)conj_q_flip_Q[i] = - q_flip_Q[i];
        Sig_sample_Q.push_back(Sig_Q[down_sample * i]);
    }
    vector<double>  syncSig_sample_I, syncSig_sample_Q;
    int L_syncSig_sample = ceil(syncSig_I.size()/down_sample);
    syncSig_sample_I.reserve(L_syncSig_sample); syncSig_sample_Q.reserve(L_syncSig_sample);
    for(int i = 0; i < L_syncSig_sample; i++){ 
        syncSig_sample_I.push_back(syncSig_I[down_sample * i]); // (X)conj_q_flip_Q[i] = - q_flip_Q[i];
        syncSig_sample_Q.push_back(syncSig_Q[down_sample * i]);
    }
    vector<int> coarse_pilot_position_sample;
    coarseTO_estimator_v2(Sig_sample_I, Sig_sample_Q, L_Sig_sample, syncSig_sample_I, syncSig_sample_Q, L_syncSig_sample ,new_sps, y1_I, y1_Q, coarse_pilot_position_sample);
    
    int number_of_bursts;
    number_of_bursts = coarse_pilot_position_sample.size();
    
    vector<int> coarse_pilot_position;
    coarse_pilot_position.reserve(number_of_bursts);
    for(int i = 0; i < number_of_bursts; i++){ 
        coarse_pilot_position.push_back(coarse_pilot_position_sample[i] * down_sample); // (X)conj_q_flip_Q[i] = - q_flip_Q[i];
    }
    t12 = clock();       
    // printf("Course TO estimation is done in %1f(sec).\n", (t12-t11)/(double)(CLOCKS_PER_SEC));
    
    // for(int j = 0; j < coarse_pilot_position.size(); j++){    
    //         printf("[%d]: %d", j, coarse_pilot_position[j]);
    // }
#endif  
#ifdef __MODE_4__  // [MODE4]
    vector<double>  y1_I, y1_Q;
    vector<int> coarse_pilot_position; // start_burst_position or pilot_position
    vector<int> end_burst_position;
    y1_I.reserve(SigLength_afterResample); y1_Q.reserve(SigLength_afterResample);
    t11 = clock(); 
    coarseTO_estimator_v3(Sig_I, Sig_Q, SigLength_afterResample, syncSig_I, syncSig_Q, L_syncSig ,new_sps, y1_I, y1_Q, coarse_pilot_position, end_burst_position); 
    int number_of_bursts;
    number_of_bursts = coarse_pilot_position.size();
    t12 = clock();       
    // printf("Course TO estimation is done in %1f(sec).\n", (t12-t11)/(double)(CLOCKS_PER_SEC));
#endif  
#ifdef __MODE_5__  // [MODE5][faster version of MODE4]
    vector<double>  y1_I, y1_Q;
    t11 = clock(); 
    int down_sample = 1;
    vector<double>  Sig_sample_I, Sig_sample_Q;
    int L_Sig_sample = ceil(Sig_I.size()/down_sample);
    Sig_sample_I.reserve(L_Sig_sample); Sig_sample_Q.reserve(L_Sig_sample);
    for(int i = 0; i < L_Sig_sample; i++){ 
        Sig_sample_I.push_back(Sig_I[down_sample * i]); // (X)conj_q_flip_Q[i] = - q_flip_Q[i];
        Sig_sample_Q.push_back(Sig_Q[down_sample * i]);
    }
    vector<double>  syncSig_sample_I, syncSig_sample_Q;
    int L_syncSig_sample = ceil(syncSig_I.size()/down_sample);
    syncSig_sample_I.reserve(L_syncSig_sample); syncSig_sample_Q.reserve(L_syncSig_sample);
    for(int i = 0; i < L_syncSig_sample; i++){ 
        syncSig_sample_I.push_back(syncSig_I[down_sample * i]); // (X)conj_q_flip_Q[i] = - q_flip_Q[i];
        syncSig_sample_Q.push_back(syncSig_Q[down_sample * i]);
    }
    vector<int> coarse_pilot_position_sample;
    vector<int> end_burst_position_sample;
    coarseTO_estimator_v3(Sig_sample_I, Sig_sample_Q, L_Sig_sample, syncSig_sample_I, syncSig_sample_Q, L_syncSig_sample ,new_sps, y1_I, y1_Q, coarse_pilot_position_sample, end_burst_position_sample); 
    int number_of_bursts;
    number_of_bursts = coarse_pilot_position_sample.size();
    
    vector<int> coarse_pilot_position; // start_burst_position or pilot_position
    vector<int> end_burst_position;
    coarse_pilot_position.reserve(number_of_bursts);
    end_burst_position.reserve(number_of_bursts);
    for(int i = 0; i < number_of_bursts; i++){ 
        coarse_pilot_position.push_back(coarse_pilot_position_sample[i] * down_sample); // (X)conj_q_flip_Q[i] = - q_flip_Q[i];
        end_burst_position.push_back(end_burst_position_sample[i] * down_sample); // (X)conj_q_flip_Q[i] = - q_flip_Q[i];
    }

    // for(int j = 0; j < coarse_pilot_position.size(); j++){    
    //     printf("%d %d ", coarse_pilot_position[j], end_burst_position[j]);
    //     //printf("%d ",  end_burst_position[j] - coarse_pilot_position[j]);
    // }

    y1_I.reserve(SigLength_afterResample); y1_Q.reserve(SigLength_afterResample);
    t12 = clock();       
    // printf("Course TO estimation is done in %1f(sec).\n", (t12-t11)/(double)(CLOCKS_PER_SEC));
#endif 
#ifdef __MODE_6__  // [MODE5][faster version of MODE4]
    vector<double>  y1_I, y1_Q;
    t11 = clock(); 
    int down_sample = 1;
    vector<double>  Sig_sample_I, Sig_sample_Q;
    int L_Sig_sample = ceil(Sig_I.size()/down_sample);
    Sig_sample_I.reserve(L_Sig_sample); Sig_sample_Q.reserve(L_Sig_sample);
    for(int i = 0; i < L_Sig_sample; i++){ 
        Sig_sample_I.push_back(Sig_I[down_sample * i]); // (X)conj_q_flip_Q[i] = - q_flip_Q[i];
        Sig_sample_Q.push_back(Sig_Q[down_sample * i]);
    }
    vector<double>  syncSig_sample_I, syncSig_sample_Q;
    int L_syncSig_sample = ceil(syncSig_I.size()/down_sample);
    syncSig_sample_I.reserve(L_syncSig_sample); syncSig_sample_Q.reserve(L_syncSig_sample);
    for(int i = 0; i < L_syncSig_sample; i++){ 
        syncSig_sample_I.push_back(syncSig_I[down_sample * i]); // (X)conj_q_flip_Q[i] = - q_flip_Q[i];
        syncSig_sample_Q.push_back(syncSig_Q[down_sample * i]);
    }
    vector<int> coarse_pilot_position_sample;
    vector<int> end_burst_position_sample;
    coarseTO_estimator_v3(Sig_sample_I, Sig_sample_Q, L_Sig_sample, syncSig_sample_I, syncSig_sample_Q, L_syncSig_sample ,new_sps, y1_I, y1_Q, coarse_pilot_position_sample, end_burst_position_sample); 
    int number_of_bursts;
    number_of_bursts = coarse_pilot_position_sample.size();
    
    vector<int> coarse_pilot_position; // start_burst_position or pilot_position
    vector<int> end_burst_position;
    coarse_pilot_position.reserve(number_of_bursts);
    end_burst_position.reserve(number_of_bursts);
    for(int i = 0; i < number_of_bursts; i++){ 
        coarse_pilot_position.push_back(coarse_pilot_position_sample[i] * down_sample); // (X)conj_q_flip_Q[i] = - q_flip_Q[i];
        end_burst_position.push_back(end_burst_position_sample[i] * down_sample); // (X)conj_q_flip_Q[i] = - q_flip_Q[i];
    }

    // for(int j = 0; j < coarse_pilot_position.size(); j++){    
    //     printf("%d %d ", coarse_pilot_position[j], end_burst_position[j]);
    //     //printf("%d ",  end_burst_position[j] - coarse_pilot_position[j]);
    // }

    y1_I.reserve(SigLength_afterResample); y1_Q.reserve(SigLength_afterResample);
    t12 = clock();       
    // printf("Course TO estimation is done in %1f(sec).\n", (t12-t11)/(double)(CLOCKS_PER_SEC));
#endif 
    ////////////////////// Define buffer and allocate memory //////////////////////
    // for pilot
    vector<double> b_i_hat_p_buffer;
    b_i_hat_p_buffer.reserve(Symbol_Length * number_of_bursts);
    vector<double> b_q_hat_p_buffer;
    b_q_hat_p_buffer.reserve(Symbol_Length * number_of_bursts);

    // for pilot + data
    vector<double> b_i_hat_d_buffer;
    b_i_hat_d_buffer.reserve(27); // 因為還不知需多少記憶體，所以先隨意allocate
    vector<double> b_q_hat_d_buffer;
    b_q_hat_d_buffer.reserve(27); // 因為還不知需多少記憶體，所以先隨意allocate

    double time_recording_buffer[5][number_of_bursts];
    // 1st row: record the time of Get process data in each burst
    // 2nd row: record the time of CFO, PHO, TO estimation in each burst
    // 3rd row: record the time of Reget databurst in each burst
    // 4th row: record the time of CFO, PHO compensation in each burst
    // 5th row: record the time of OQPSK aftendRx process in each burst
    tIteration_start = clock(); 
    int L_b_hat_d_burst; 
    int K;
    int Lv;
    vector<int> demd_symbol_per_burst; // record the number of demod symbol in each burst
    for (int i = 0; i < number_of_bursts; i++){
        //////////////////////   Get processed data   ////////////////////// 
    #ifdef __MODE_1__ 
        K = 2000; 
        Lv = 2 * K + 1; // Length of v
    #endif 
    #ifdef __MODE_2__ 
        K = 2000; 
        Lv = 2 * K + 1; // Length of v
    #endif 
    #ifdef __MODE_3__ 
        K = 2000; 
        Lv = 2 * K + 1; // Length of v
    #endif 
    #ifdef __MODE_4__ 
        Lv = end_burst_position[i] - coarse_pilot_position[i] + 1;
        K = round(Lv/2);
    #endif     
    #ifdef __MODE_5__ 
        Lv = end_burst_position[i] - coarse_pilot_position[i] + 1;
        K = round(Lv/2);
    #endif  
    #ifdef __MODE_6__ 
        K = 870; 
        Lv = 2 * K + 1; // Length of v
    #endif  

        vector<double> v_I, v_Q;
        v_I.reserve(Lv); v_Q.reserve(Lv);
        
        int start_burst = coarse_pilot_position[i];
        tIteration_1 = clock();
        for (int i = 1; i <= Lv; i++){
            v_I.push_back(Sig_I[i + start_burst - K - 2]); //v_I[i-1] = Sig_I[i + coarse_pilot_position - K - 2];
            v_Q.push_back(Sig_Q[i + start_burst - K - 2]);//v_Q[i-1] = Sig_Q[i + coarse_pilot_position - K - 2];
        }
        tIteration_2 = clock();    
        //printf("Get processed data in %1f(sec).\n", (tIteration_2-tIteration_1)/(double)(CLOCKS_PER_SEC));
        time_recording_buffer[0][i] = (tIteration_2-tIteration_1)/(double)(CLOCKS_PER_SEC);

        //////////////////////  CFO, PHO, TO estimation  //////////////////////
        vector<double> CFOhat_PHOhat_d;
        CFOhat_PHOhat_d.reserve(3); //cfo_pho_hat = [pho_hat cfo_hat d]

        tIteration_1 = clock();
        
    // [MODE A] original version (error pilot: 14/0.8s data)
    #ifdef __MODE_A__  
        CFOPHOTO_estimator(CFOhat_PHOhat_d, v_I, v_Q, Lv, syncSig_I, syncSig_Q, L_syncSig, K, Ts_USRP);
    #endif  
    // [MODE B] speed up version [reduce the needed while loop and for loop. Discriminant is also added to break the loop earlier] (error pilot: 101/0.8s data)
    #ifdef __MODE_B__  
        CFOPHOTO_estimator_v2(CFOhat_PHOhat_d, v_I, v_Q, Lv, syncSig_I, syncSig_Q, L_syncSig, K, Ts_USRP);
    #endif      
    // [MODE C] speed up version [only do one while loop] (error pilot: 78/0.8s data)
    #ifdef __MODE_C__  
        CFOPHOTO_estimator_v3(CFOhat_PHOhat_d, v_I, v_Q, Lv, syncSig_I, syncSig_Q, L_syncSig, K, Ts_USRP);
    #endif
    #ifdef __MODE_D__  
        CFOPHOTO_estimator_v4(CFOhat_PHOhat_d, v_I, v_Q, Lv, syncSig_I, syncSig_Q, L_syncSig, K, Ts_USRP);
    #endif          
        v_I.clear(); v_Q.clear();
        tIteration_2 = clock();    
        //printf("CFO, PHO, TO estimation is done in %1f(sec).\n", (tIteration_2-tIteration_1)/(double)(CLOCKS_PER_SEC));
        time_recording_buffer[1][i] = (tIteration_2-tIteration_1)/(double)(CLOCKS_PER_SEC);
    
        double cfo_hat, pho_hat, d;
        cfo_hat = CFOhat_PHOhat_d[0]; 
        pho_hat = CFOhat_PHOhat_d[1]; 
        d = CFOhat_PHOhat_d[2];
        CFOhat_PHOhat_d.clear();

    #ifdef __DEBUG_MAIN__  
        cout << "burst" << i+1 << "\n";
        cout << "cfo_hat: " << cfo_hat << "\n"; //cfo_hat = 0.01012;
        cout << "pho_hat: " << pho_hat << "\n"; //pho_hat = -2.73261;
        cout << "d: " << d << "\n";             //d = -3065;
    #endif           
        ////////////////////// Reget the correct received pilot //////////////////////   
        int end_burst_original;
    #ifdef __MODE_1__ 
        end_burst_original = start_burst + K;
    #endif 
    #ifdef __MODE_2__ 
        end_burst_original = start_burst + K;    
    #endif 
    #ifdef __MODE_3__ 
        end_burst_original = start_burst + K;
    #endif 
    #ifdef __MODE_4__ 
        end_burst_original = end_burst_position[i];
    #endif 
    #ifdef __MODE_5__ 
        end_burst_original = end_burst_position[i];
    #endif 
    #ifdef __MODE_6__ 
        end_burst_original = end_burst_position[i];
        //cout <<end_burst_position[i]<<endl;
    #endif 
        
        int exact_pilot_position = start_burst + d;
        vector<double> burst_interval;
        burst_interval.reserve(2);
        burst_interval.push_back(start_burst); burst_interval.push_back(end_burst_original); 
        vector<double> RegetReceived_pilot_I, RegetReceived_pilot_Q;// reserve the memory in "get_databurst.h"
        vector<double> Regetdata_burst_I, Regetdata_burst_Q; // reserve the memory in "get_databurst.h"
        tIteration_1 = clock();    
        get_databurst(RegetReceived_pilot_I, RegetReceived_pilot_Q, Regetdata_burst_I, Regetdata_burst_Q, burst_interval, Sig_I, Sig_Q, syncSig_I, syncSig_Q, exact_pilot_position, new_sps);
        tIteration_2 = clock();    
        //printf("Reget databurst is done in %1f(sec).\n", (tIteration_2-tIteration_1)/(double)(CLOCKS_PER_SEC));
        time_recording_buffer[2][i] = (tIteration_2-tIteration_1)/(double)(CLOCKS_PER_SEC);
    
        int L_regetSig = Regetdata_burst_I.size();
        Sig_I.clear(); Sig_Q.clear(); 
        burst_interval.clear();

        ////////////////////// CFOPHOTO compensation ////////////////////// 
        // for pilot
        // int L_regetpilot = L_syncSig;
        // vector<double> Received_pilot_compen_I, Received_pilot_compen_Q;
        // Received_pilot_compen_I.reserve(L_regetpilot); Received_pilot_compen_Q.reserve(L_regetpilot); 
        // CFOPHOcompen(Received_pilot_compen_I, Received_pilot_compen_Q, RegetReceived_pilot_I, RegetReceived_pilot_Q, L_regetpilot, pho_hat, cfo_hat, 1/new_fs);
        // RegetReceived_pilot_I.clear(); RegetReceived_pilot_Q.clear();  

        // for pilot + data
        tIteration_1 = clock(); 
        vector<double> data_burst_compen_I, data_burst_compen_Q;
        data_burst_compen_I.reserve(L_regetSig); data_burst_compen_Q.reserve(L_regetSig); 
        CFOPHOcompen(data_burst_compen_I, data_burst_compen_Q, Regetdata_burst_I, Regetdata_burst_Q, L_regetSig, pho_hat, cfo_hat, 1/new_fs);
        Regetdata_burst_I.clear(); Regetdata_burst_Q.clear();  
        tIteration_2 = clock();    
        //printf("CFO, PHO compensation is done in %1f(sec).\n", (tIteration_2-tIteration_1)/(double)(CLOCKS_PER_SEC));       
        time_recording_buffer[3][i] = (tIteration_2-tIteration_1)/(double)(CLOCKS_PER_SEC);
    
        //////////////////////   OQPSK aftend Rx   //////////////////////
        // for pilot
        // vector<double> b_i_hat_p, b_q_hat_p;
        // b_i_hat_p.reserve(Symbol_Length); b_q_hat_p.reserve(Symbol_Length); 
        // OQPSK_aftendRx(b_i_hat_p, b_q_hat_p, Received_pilot_compen_I, Received_pilot_compen_Q, L_regetpilot, Pulseshaping_I, Pulseshaping_Q, L_pulseshaping, FilterSpanInSymbols, new_sps);
        // Received_pilot_compen_I.clear(); Received_pilot_compen_Q.clear();
        // int L_b_i_hat_p = b_i_hat_p.size();

        // for pilot + data
        tIteration_1 = clock();
        vector<double> b_i_hat_d, b_q_hat_d;
        b_i_hat_d.reserve(L_regetSig); b_q_hat_d.reserve(L_regetSig); 
        OQPSK_aftendRx(b_i_hat_d, b_q_hat_d, data_burst_compen_I, data_burst_compen_Q, L_regetSig, Pulseshaping_I, Pulseshaping_Q, L_pulseshaping, FilterSpanInSymbols, new_sps);
        data_burst_compen_I.clear(); data_burst_compen_Q.clear();

        Pulseshaping_I.clear(); Pulseshaping_Q.clear();     
        ////////////////////// save values into buffer ////////////////////// 
        // for pilot
        // for(int j = 0; j < Symbol_Length; j++){
        //     b_i_hat_p_buffer.push_back(b_i_hat_p[j]);
        // }
        // b_i_hat_p.clear();
        // for(int j = 0; j < Symbol_Length; j++){
        //     b_q_hat_p_buffer.push_back(b_q_hat_p[j]);
        // }
        // b_q_hat_p.clear();

        // for pilot + data
        L_b_hat_d_burst = b_i_hat_d.size();
        for(int j = 0; j < b_i_hat_d.size(); j++){
            b_i_hat_d_buffer.push_back(b_i_hat_d[j]);
        }
        b_i_hat_d.clear();
        for(int j = 0; j < b_q_hat_d.size(); j++){
            b_q_hat_d_buffer.push_back(b_q_hat_d[j]);
        }
        b_q_hat_d.clear(); 
        tIteration_2 = clock();    
        //printf("OQPSK aftendRx process is done in %1f(sec).\n", (tIteration_2-tIteration_1)/(double)(CLOCKS_PER_SEC));       
        time_recording_buffer[4][i] = (tIteration_2-tIteration_1)/(double)(CLOCKS_PER_SEC);    
        demd_symbol_per_burst.push_back(L_b_hat_d_burst); // record the number of demod symbol in each burst
    }// end for    
    tIteration_end = clock(); 
        
    //////////////  DecisionProcess  ////////////////
    // for pilot   
    // vector<double> b_hat_p;
    // int L_b_buffer_p = b_i_hat_p_buffer.size();
    // int L_b_hat_p = L_b_buffer_p * 2;
    // b_hat_p.reserve(L_b_hat_p);
    // DecisionProcess(b_hat_p, L_b_hat_p, b_i_hat_p_buffer, b_q_hat_p_buffer, L_b_buffer_p);

    // for pilot + data
    t13 = clock();
    vector<double> b_hat_d;
    int L_b_buffer_d = b_i_hat_d_buffer.size();
    int L_b_hat_d = L_b_buffer_d * 2;
    b_hat_d.reserve(L_b_hat_d);
    DecisionProcess(b_hat_d, L_b_hat_d, b_i_hat_d_buffer, b_q_hat_d_buffer, L_b_buffer_d);
    t14 = clock();
    //printf("Decision process is done in %1f(sec).\n", (t14-t13)/(double)(CLOCKS_PER_SEC));
    
    //////////////  Reorganize bit order  ////////////////
    // for pilot
    // vector<double> m_hat_p;
    // int L_m_hat_p = L_b_hat_p;
    // m_hat_p.reserve(L_m_hat_p);
    // bitReorganizationRx(m_hat_p, b_hat_p, L_m_hat_p);
    // b_hat_p.clear(); 
    // for pilot + data
    t15 = clock();
    vector<double> m_hat_d;
    int L_m_hat_d = L_b_hat_d;
    m_hat_d.reserve(L_m_hat_d);
    bitReorganizationRx(m_hat_d, b_hat_d, L_m_hat_d);
    t16 = clock();
    //printf("Bit reorganization is done in %1f(sec).\n", (t16-t15)/(double)(CLOCKS_PER_SEC));    
    b_hat_d.clear(); 

    //////////////  Symbol demapping  ////////////////
    // for pilot
    // vector<double> ReceivedSym_p;
    // int L_ReceivedSym_p = L_b_hat_p/2;
    // ReceivedSym_p.reserve(L_ReceivedSym_p);
    // symbol_demapping(ReceivedSym_p, L_ReceivedSym_p, m_hat_p);        
    // m_hat_p.clear();
    // for pilot + data
    t17 = clock();
    vector<double> ReceivedSym_d;
    int L_ReceivedSym_d = L_b_hat_d/2;
    ReceivedSym_d.reserve(L_ReceivedSym_d);
    symbol_demapping(ReceivedSym_d, L_ReceivedSym_d, m_hat_d);        
    m_hat_d.clear();
    t18 = clock();
    total_end = clock();

    //  cout << ReceivedSym_d.size() <<endl;
    // int sum_demod_sym = 0;
    // for(int i = 0; i < demd_symbol_per_burst.size(); i++){    
    //     cout <<demd_symbol_per_burst[i] <<endl;
    //     sum_demod_sym = sum_demod_sym + demd_symbol_per_burst[i];
    // }
    // cout << sum_demod_sym << endl;
    //printf("Symbol demapping is done in %1f(sec).\n", (t18-t17)/(double)(CLOCKS_PER_SEC));              
    //printf("All process is done in %1f(sec).\n", (total_end-total_start)/(double)(CLOCKS_PER_SEC));    
    
    ////////////// Get received pilot from  received symbol //////////////
    // Number of processing data in each burst, K, is set in the previous iteration
    // Length of L_b_hat_d_burst in each burst, L_b_hat_d, is record in the previous iteration
    vector<double> vec_b_i_hat_p_K, vec_b_q_hat_p_K;
    vector<double> ReceivedSym_p_K;
    int L_vec_b =  number_of_bursts * L_ReceivedSym_d;
    vec_b_i_hat_p_K.reserve(L_vec_b); vec_b_q_hat_p_K.reserve(L_vec_b);
    ReceivedSym_p_K.reserve(L_vec_b);
#ifdef __MODE_1__ 
    for(int i = 1; i <= number_of_bursts; i++){
        for(int j = 0; j < Symbol_Length; j++){
            vec_b_i_hat_p_K.push_back(b_i_hat_d_buffer[L_b_hat_d_burst * (i-1) + j]);
            vec_b_q_hat_p_K.push_back(b_q_hat_d_buffer[L_b_hat_d_burst * (i-1) + j]);
            ReceivedSym_p_K.push_back(ReceivedSym_d[L_b_hat_d_burst * (i-1) + j]);
        }    
    }
#endif 
#ifdef __MODE_2__ 
    for(int i = 1; i <= number_of_bursts; i++){
        for(int j = 0; j < Symbol_Length; j++){
            vec_b_i_hat_p_K.push_back(b_i_hat_d_buffer[L_b_hat_d_burst * (i-1) + j]);
            vec_b_q_hat_p_K.push_back(b_q_hat_d_buffer[L_b_hat_d_burst * (i-1) + j]);
            ReceivedSym_p_K.push_back(ReceivedSym_d[L_b_hat_d_burst * (i-1) + j]);
        }    
    }
#endif 
#ifdef __MODE_3__ 
    for(int i = 1; i <= number_of_bursts; i++){
        for(int j = 0; j < Symbol_Length; j++){
            vec_b_i_hat_p_K.push_back(b_i_hat_d_buffer[L_b_hat_d_burst * (i-1) + j]);
            vec_b_q_hat_p_K.push_back(b_q_hat_d_buffer[L_b_hat_d_burst * (i-1) + j]);
            ReceivedSym_p_K.push_back(ReceivedSym_d[L_b_hat_d_burst * (i-1) + j]);
        }    
    }
#endif 
#ifdef __MODE_4__ 
    int count_symbol;
    for(int i = 1; i <= number_of_bursts; i++){
        if(i == 1){
            count_symbol = 0;
        }
        else{
            count_symbol = count_symbol + demd_symbol_per_burst[i-2];
        } 
        for(int j = 0; j < Symbol_Length; j++){  
            vec_b_i_hat_p_K.push_back(b_i_hat_d_buffer[count_symbol + j]);
            vec_b_q_hat_p_K.push_back(b_q_hat_d_buffer[count_symbol + j]);
            ReceivedSym_p_K.push_back(ReceivedSym_d[count_symbol + j]);
        }    
    }
#endif  
#ifdef __MODE_5__ 
    int count_symbol;
    for(int i = 1; i <= number_of_bursts; i++){
        if(i == 1){
            count_symbol = 0;
        }
        else{
            count_symbol = count_symbol + demd_symbol_per_burst[i-2];
        } 
        for(int j = 0; j < Symbol_Length; j++){  
            vec_b_i_hat_p_K.push_back(b_i_hat_d_buffer[count_symbol + j]);
            vec_b_q_hat_p_K.push_back(b_q_hat_d_buffer[count_symbol + j]);
            ReceivedSym_p_K.push_back(ReceivedSym_d[count_symbol + j]);
        }    
    }
#endif     
#ifdef __MODE_6__ 
    int count_symbol;
    for(int i = 1; i <= number_of_bursts; i++){
        if(i == 1){
            count_symbol = 0;
        }
        else{
            count_symbol = count_symbol + demd_symbol_per_burst[i-2];
        } 
        for(int j = 0; j < Symbol_Length; j++){  
            vec_b_i_hat_p_K.push_back(b_i_hat_d_buffer[count_symbol + j]);
            vec_b_q_hat_p_K.push_back(b_q_hat_d_buffer[count_symbol + j]);
            ReceivedSym_p_K.push_back(ReceivedSym_d[count_symbol + j]);
        }    
    }
#endif 

    ////////////// SNR estimation //////////////
    // only use pilots to calculate the SNR
    double SNR_dB;
    SNR_dB = SNRestimation(DataType, vec_b_i_hat_p_K, vec_b_q_hat_p_K);
    //cout << SNR_dB <<"\n";
    //printf("SNR estimation is done.\n");

    // ////////////// SER and BER estimation //////////////
    vector<double> SER_BER_errorpilot;
    SER_BER_errorpilot.reserve(3);
    ErrorRateCalculation(SER_BER_errorpilot, DataType, pilotPattern_vec, ReceivedSym_p_K);
    //ErrorRateCalculation(SER_BER_errorpilot, DataType, pilotSym_I, pilotSym_Q, b_i_hat_p_buffer, b_q_hat_p_buffer);
    double SER, BER, errorpilot;
    SER = SER_BER_errorpilot[0];
    BER = SER_BER_errorpilot[1];
    errorpilot = SER_BER_errorpilot[2];

    // b_i_hat_p_buffer.clear(); b_q_hat_p_buffer.clear();
    ///////////// printf the time-consuming of each process /////////////
    
    //printf("Number of processing samples: %d\n", filelength);
    //printf("Sampling Frequency: %f(Hz)\n", fs_USRP); // fs_USRP = 6.25 * pow(10, 6); 
    printf("********************************************************************************\n");
    printf("************ Time Recording ************\n");
    // printf("Data conversion is done in %1f(sec).\n", (t2-t1)/(double)(CLOCKS_PER_SEC)); // 不需算進總執行時間
    printf("DC offset of data is removed in %1f(sec).\n", (t4-t3)/(double)(CLOCKS_PER_SEC));
    printf("Down conversion is done in %1f(sec).\n", (t6-t5)/(double)(CLOCKS_PER_SEC));
    printf("Data resample is done in %1f(sec).\n", (t8-t7)/(double)(CLOCKS_PER_SEC));
    printf("OQPSK frontendTx process is done in %f(sec).\n", (t10-t9)/(double)(CLOCKS_PER_SEC)); 
    printf("Course TO estimation is done in %1f(sec).\n", (t12-t11)/(double)(CLOCKS_PER_SEC));
    printf("CFO, PHO, TO Processing of all bursts is done in %1f(sec)\n", (tIteration_end - tIteration_start)/(double)(CLOCKS_PER_SEC));  
    printf("Decision process is done in %1f(sec).\n", (t14-t13)/(double)(CLOCKS_PER_SEC));
    printf("Bit reorganization is done in %1f(sec).\n", (t16-t15)/(double)(CLOCKS_PER_SEC));    
    printf("Symbol demapping is done in %1f(sec).\n", (t18-t17)/(double)(CLOCKS_PER_SEC));              
    printf("All process is done in %1f(sec).\n", (total_end-total_start)/(double)(CLOCKS_PER_SEC));              
    
    cout << "\n";
    printf("SNR estimation is done.\n");
    cout << "SNR_dB: " << SNR_dB << endl; 
    printf("SER and BER calculation is done.\n");
    cout << "SER: " << SER << endl; 
    
    ///////////// printf the time-consuming of each iteration /////////////    
    // cout << "\n";
    // printf("********************************************************************************\n");
    // printf("CFO, PHO, TO Processing of all burst is done in %1f(sec)\n", (tIteration_end - tIteration_start)/(double)(CLOCKS_PER_SEC));  
    // printf("************ CFO, PHO, TO Processing of each burst ************\n");  
    // for(int j = 0; j < number_of_bursts; j++){
    //     printf("burst %d\n", j+1);
    //     printf("Get process data is done in %f(sec)\n", time_recording_buffer[0][j]);
    //     printf("CFO, PHO, TO estimation is done in %f(sec)\n", time_recording_buffer[1][j]);
    //     printf("Reget databurst in %f(sec)\n", time_recording_buffer[2][j]);
    //     printf("CFO, PHO compensation is done in %f(sec)\n", time_recording_buffer[3][j]);
    //     printf("OQPSK aftendRx process is done in %1f(sec).\n", (tIteration_2-tIteration_1)/(double)(CLOCKS_PER_SEC));            
    // } 
    

    ////////////// printf symbol_demapping result //////////////
    printf("********************************************************************************\n");
    // cout << "************ symbol_demapping result(pilot only) ************" << "\n";
    // //for pilot
    // for (int i = 0; i < L_ReceivedSym_p; i++){
    //     printf("[%d]:%d ", i, int(ReceivedSym_p[i]));
    // }
    
    //for pilot + data    
    // cout << "\n";
    cout << "************ symbol_demapping result(pilot and data) ************" << "\n";
    // for (int i = 0; i < L_ReceivedSym_d; i++){
    //     printf("[%d]:%d ", i, int(ReceivedSym_d[i]));
    // }
    // for (int i = 0; i < 10; i++){
    //     printf("[%d]:%d ", i, int(ReceivedSym_d[i]));
    // }
    // for(int i = 0;i < demd_symbol_per_burst.size();i++){
    //     printf("%d ", demd_symbol_per_burst[i]);
    // }
   
    /////////////////////// write OQPSK demod result into file  //////////////////////////// 
    ofstream OQPSK_demod_result;
    OQPSK_demod_result.open ("OQPSK_demod_result.txt");
    OQPSK_demod_result << "OQPSK_demod_result" << endl;
    int count;
    int nextline_count;
    for(int j = 0; j < coarse_pilot_position.size(); j++){
        //OQPSK_demod_result << "fc:" << fc << " burst" << j+1 << "\n";
        OQPSK_demod_result << "fc:" << fc  << "\n";
        //OQPSK_demod_result << "burst interval: " << coarse_pilot_position[j] << " ~ " << end_burst_position[j] << "\n";
        OQPSK_demod_result << "burst interval: " << coarse_pilot_position[j] << " ~ " << coarse_pilot_position[j]+K << "\n";
        if(j==0){
            count = 0;
        }
        nextline_count = 0;
        for(int i = 0; i < demd_symbol_per_burst[j] ;i++){
            OQPSK_demod_result << ReceivedSym_d[count] << " ";
            count = count + 1;
            nextline_count = nextline_count + 1;
            if( nextline_count == 50){
            OQPSK_demod_result << "\n"; 
            nextline_count = 0;
            }
        }
        OQPSK_demod_result << "\n"; 
    }
    OQPSK_demod_result.close();

    return 0;
}

