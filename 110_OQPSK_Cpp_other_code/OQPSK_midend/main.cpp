// used burst 1 "pilot"
// correct
// the demapping symbol is correct

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define pi 3.141592653589793238462643383279502884
#include <time.h>
#include <fftw3.h>

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

#include "Flip_vec.h"       // used in "CFOPHOTO_estimator_test2.h"
#include "tmp_cfo_compen.h" // used in "CFOPHOTO_estimator_test2.h"
#include "CFOPHOTO_estimator.h"

#include "get_databurst.h"


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
    //filename1 = "RegetReceived_pilot_Re.txt"; 
    filename1 = "RegetReceived_pilot_Re_burst1.txt"; 
    readtxt2array(filename1, filelength, array1);

    //double array2[filelength];
    double *array2 = (double*)malloc(filelength * sizeof(double)); 
    const char *filename2;
    //filename2 = "RegetReceived_pilot_Im.txt"; 
    filename2 = "RegetReceived_pilot_Im_burst1.txt"; 
    readtxt2array(filename2, filelength, array2);

    //double array2[filelength];
    int filelength3 = 157;
    double *array3 = (double*)malloc(filelength3 * sizeof(double)); 
    const char *filename3;
    filename3 = "Pulseshaping.txt"; 
    readtxt2array(filename3, filelength3, array3);

    filelength = 6001;
    double *v_Re = (double*)malloc(filelength * sizeof(double)); 
    const char *filename4;
    filename4 = "v_burst1_K=3000_Re.txt"; 
    readtxt2array(filename4, filelength, v_Re);
    
    filelength = 6001;
    double *v_Im = (double*)malloc(filelength * sizeof(double)); 
    const char *filename5;
    filename5 = "v_burst1_K=3000_Im.txt"; 
    readtxt2array(filename5, filelength, v_Im);

    filelength = 858;
    double *syncSig_Re = (double*)malloc(filelength * sizeof(double)); 
    const char *filename6;
    filename6 = "syncSig_Re.txt"; 
    readtxt2array(filename6, filelength, syncSig_Re);

    filelength = 858;
    double *syncSig_Im = (double*)malloc(filelength * sizeof(double)); 
    const char *filename7;
    filename7 = "syncSig_Im.txt"; 
    readtxt2array(filename7, filelength, syncSig_Im);

    filelength = 532480;
    double *Sig_Re = (double*)malloc(filelength * sizeof(double)); 
    const char *filename8;
    filename8 = "Sig_Re.txt"; 
    readtxt2array(filename8, filelength, Sig_Re);
    
    filelength = 532480;
    double *Sig_Im = (double*)malloc(filelength * sizeof(double)); 
    const char *filename9;
    filename9 = "Sig_Im.txt"; 
    readtxt2array(filename9, filelength, Sig_Im);
    /*************   Change array data type to vector   **************/
    /*
    int L_regetpilot = 858;
    vector<double> RegetReceived_pilot_I, RegetReceived_pilot_Q;
    RegetReceived_pilot_I.reserve(L_regetpilot); RegetReceived_pilot_Q.reserve(L_regetpilot);
    arraytovec(RegetReceived_pilot_I, RegetReceived_pilot_Q, array1, array2, L_regetpilot); 
    free(array1); free(array2);
    */

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
    
    int Lv = 6001;
    vector<double>  v_I(v_Re, v_Re + Lv); 
    vector<double>  v_Q(v_Im, v_Im + Lv); 

    int L_syncSig = 858;
    vector<double>  syncSig_I(syncSig_Re, syncSig_Re + L_syncSig); 
    vector<double>  syncSig_Q(syncSig_Im, syncSig_Im + L_syncSig);    

    int L_Sig = 532480;
    vector<double>  Sig_I(Sig_Re, Sig_Re + L_Sig); 
    vector<double>  Sig_Q(Sig_Im, Sig_Im + L_Sig);    

    /////////////// some parameters ///////////////
    int K = 3000;
    //double Ts_USRP = 1.6 * pow(10, -7); ;
    vector<double> CFOhat_PHOhat_d;
    CFOhat_PHOhat_d.reserve(3); //cfo_pho_hat = [pho_hat cfo_hat d]

    //////////////////////  CFO, PHO, TO estimation  //////////////////////
    //CFOPHOTO_estimator(v_I, v_Q, Lv, syncSig_I, syncSig_Q, L_syncSig, K, Ts_USRP);
    CFOPHOTO_estimator(CFOhat_PHOhat_d, v_I, v_Q, Lv, syncSig_I, syncSig_Q, L_syncSig, K, Ts_USRP);
    double cfo_hat, pho_hat, d;
    cfo_hat = CFOhat_PHOhat_d[0]; 
    pho_hat = CFOhat_PHOhat_d[1]; 
    d = CFOhat_PHOhat_d[2];
    cout << "cfo_hat: " << cfo_hat << "\n";
    cout << "pho_hat: " << pho_hat << "\n";
    cout << "d: " << d << "\n";

    CFOhat_PHOhat_d.clear();

   ////////////////////// Reget the correct received pilot //////////////////////  
    double coarse_pilot_position_burst1 = 11228;
    double start_burst = coarse_pilot_position_burst1;
    double end_burst_original =  38164;  //double end_burst_original = start_burst + K;
    double exact_pilot_position = start_burst + d;
    
    vector<double> burst_interval;
    burst_interval.reserve(2);
    burst_interval.push_back(start_burst); burst_interval.push_back(end_burst_original); 

    vector<double> RegetReceived_pilot_I, RegetReceived_pilot_Q;
    RegetReceived_pilot_I.reserve(L_syncSig); RegetReceived_pilot_Q.reserve(L_syncSig); 
    vector<double> Regetdata_burst_I, Regetdata_burst_Q; // reserve the memory in the function

    get_databurst(RegetReceived_pilot_I, RegetReceived_pilot_Q, Regetdata_burst_I, Regetdata_burst_Q, burst_interval, Sig_I, Sig_Q, syncSig_I, syncSig_Q, exact_pilot_position, new_sps);

    int L_regetSig = Regetdata_burst_I.size();
    //test_printf_2_vec(RegetReceived_pilot_I, RegetReceived_pilot_Q, 10);

    //////////////////////   CFOPHOTO compensation   //////////////////////
    //double pho_hat = -2.7326;
    //double cfo_hat = 0.0101;
    double new_fs = 6.656 * pow(10, 6);  
    int L_regetpilot = L_syncSig;
    vector<double> Received_pilot_compen_I, Received_pilot_compen_Q;
    Received_pilot_compen_I.reserve(L_regetpilot); Received_pilot_compen_Q.reserve(L_regetpilot); 

    CFOPHOcompen(Received_pilot_compen_I, Received_pilot_compen_Q, RegetReceived_pilot_I, RegetReceived_pilot_Q, L_regetpilot, pho_hat, cfo_hat, 1/new_fs);
    RegetReceived_pilot_I.clear(); RegetReceived_pilot_Q.clear();  

    //////////////////////   OQPSK aftend Rx   //////////////////////
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
   
    //test_printf_1_vec(ReceivedSym, 27);

    //test_printf_1_vec(ReceivedSym, 10);
    //test_printf_2_vec(OutSig_I, OutSig_Q, 10);
    //cout << b_i_hat.size() << '\n';
    return 0;
}
