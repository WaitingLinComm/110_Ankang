#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <iostream>
using namespace std;

#include "readtxt2array.h"
#include "arraytovec.h"
#include "CFOPHOcompen.h"
#include "OQPSK_aftendRx.h"
#include "DecisionProcess.h"
#include "bitReorganization.h"
#include "symbol_demapping.h"

#include "test_printf_2_vec.h"
#include "test_printf_1_vec.h"
int main() 
{ 
  
    /******************** Data Paremeters ********************/
    double fc = -338745; // center frequency of subcarrier
    // Tx info.
    double fsy = 0.256 * pow(10, 6);        // symbol rate  
    double Tsy = 1/fsy;                     // symbol period
    // USRP info.
    double fs_USRP = 6.25 * pow(10, 6);     // sampling frequency
    double Ts_USRP = 1/fs_USRP;             // sampling period
    double sps_USRP = fs_USRP/fsy;          // SamplePerSymbol. NOTE: USRP_sps is not interger. While sps should be even integer, so Resample Process is needed later on. 
    
    /*************** Parameters aftet Resample(Sinc Interpolation) *******************/
    float old_sps = sps_USRP;
    int new_sps = 26;             // Set by ourselves
    float N = new_sps / old_sps;  // upsample by N. NOTE: N can be non-integer 
  
    /********************     Pulse Shaping Filter Parameters   ********************/
    int FilterSpanInSymbols = 6;
    float RolloffFactor = 0.3;
    int FilterLength = FilterSpanInSymbols * new_sps + 1;

    /********************          Read File          ********************/
    int filelength = 858;
    //double array1[filelength];
    double *array1 = (double*)malloc(filelength * sizeof(double)); 
    const char *filename1;
    filename1 = "RegetReceived_pilot_Re.txt"; 
    readtxt2array(filename1, filelength, array1);

    //double array2[filelength];
    double *array2 = (double*)malloc(filelength * sizeof(double)); 
    const char *filename2;
    filename2 = "RegetReceived_pilot_Im.txt"; 
    readtxt2array(filename2, filelength, array2);

    //double array2[filelength];
    int filelength3 = 157;
    double *array3 = (double*)malloc(filelength3 * sizeof(double)); 
    const char *filename3;
    filename3 = "Pulseshaping.txt"; 
    readtxt2array(filename3, filelength3, array3);

    /*************   Change array data type to vector   **************/
    int L_regetpilot = 858;
    vector<double> RegetReceived_pilot_I, RegetReceived_pilot_Q;
    RegetReceived_pilot_I.reserve(L_regetpilot); RegetReceived_pilot_Q.reserve(L_regetpilot);
    arraytovec(RegetReceived_pilot_I, RegetReceived_pilot_Q, array1, array2, L_regetpilot); 
    free(array1); free(array2);

    // NOTE: Pulseshaping is a real value vector
    // Although Pulseshaping is a real value vector, we would like to set its imaginary part as 0
    // for the unify concern.
    int L_pulseshaping = 157;
    vector<double> Pulseshaping_I, Pulseshaping_Q;
    Pulseshaping_I.reserve(L_pulseshaping); Pulseshaping_Q.reserve(L_pulseshaping);
    
    for(int i = 0; i < L_pulseshaping; i++){
        Pulseshaping_I.push_back(array3[i]);
        // set the imaginary part of Pulseshaping as zero
        Pulseshaping_Q.push_back(0);
        }
    free(array3); //free(array2);
    //test_printf_2_vec(Pulseshaping_I, Pulseshaping_Q, 20);
    
    /*************   CFOPHOTO compensation   **************/
    double pho_hat = -2.7326;
    double cfo_hat = 0.0101;
    double new_fs = 6.656 * pow(10, 6);  

    vector<double> Received_pilot_compen_I, Received_pilot_compen_Q;
    Received_pilot_compen_I.reserve(L_regetpilot); Received_pilot_compen_Q.reserve(L_regetpilot); 

    CFOPHOcompen(Received_pilot_compen_I, Received_pilot_compen_Q, RegetReceived_pilot_I, RegetReceived_pilot_Q, L_regetpilot, pho_hat, cfo_hat, 1/new_fs);
    RegetReceived_pilot_I.clear(); RegetReceived_pilot_Q.clear();  

    /*************   OQPSK aftend Rx   **************/
    vector<double> b_i_hat, b_q_hat;
    vector<double> RxSym;
    OQPSK_aftendRx(b_i_hat, b_q_hat, Received_pilot_compen_I, Received_pilot_compen_Q, L_regetpilot, Pulseshaping_I, Pulseshaping_Q, L_pulseshaping, FilterSpanInSymbols, new_sps);
    Received_pilot_compen_I.clear(); Received_pilot_compen_Q.clear();
    Pulseshaping_I.clear(); Pulseshaping_Q.clear(); 
    int L_b_i_hat = b_i_hat.size();

    //////////////  DecisionProcess  ////////////////
    vector<double> b_hat;
    int L_b_hat = L_b_i_hat * 2;
    b_hat.reserve(L_b_hat);
    DecisionProcess(b_hat, L_b_hat, b_i_hat, b_q_hat, L_b_i_hat);
    b_i_hat.clear(); b_q_hat.clear();

    //////////////  Reorganize bit order  ////////////////
    vector<double> m_hat;
    int L_m_hat = L_b_hat;
    m_hat.reserve(L_m_hat);
    bitReorganization(m_hat, b_hat, L_m_hat);
    b_hat.clear(); 

    //////////////  Symbol demapping  ////////////////
    vector<double> ReceivedSym;
    int L_ReceivedSym = L_b_hat/2;
    ReceivedSym.reserve(L_ReceivedSym);
    symbol_demapping(ReceivedSym, L_ReceivedSym, m_hat);
    m_hat.clear();


    //test_printf_1_vec(ReceivedSym, 10);
    //test_printf_2_vec(OutSig_I, OutSig_Q, 10);
    //cout << b_i_hat.size() << '\n';
    return 0;
}
