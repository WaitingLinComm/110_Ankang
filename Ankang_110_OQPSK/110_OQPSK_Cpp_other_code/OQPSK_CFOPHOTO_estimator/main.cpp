// The code test the CFOPHOTP_estimator in main_Ankang_OQPSKv2_2.m 
// with burst 1 data

#include <iostream>
using namespace std;
#include <vector>
#include <math.h>
#define pi 3.141592653589793238462643383279502884
#include <time.h>
#include <fftw3.h>

#include "readtxt2array.h"
#include "test_printf_2_vec.h"
#include "test_printf_1_vec.h"

#include "tmp_cfo_compen.h"       // used in "CFOPHOTO_estimator.h"
#include "complex_convolution.h"  // used in "CFOPHOTO_estimator.h"
#include "Flip_vec.h"             // used in "CFOPHOTO_estimator.h"
#include "CFOPHOTO_estimator.h" 
#include "CFOPHOTO_estimator_test2.h" 
#include "get_databurst.h" 


int main()
{
    //////////////////////     Read input file(from matlab)     ////////////////////// 
    int filelength = 6001;
    double *v_Re = (double*)malloc(filelength * sizeof(double)); 
    const char *filename1;
    filename1 = "v_burst1_K=3000_Re.txt"; 
    readtxt2array(filename1, filelength, v_Re);
    
    filelength = 6001;
    double *v_Im = (double*)malloc(filelength * sizeof(double)); 
    const char *filename2;
    filename2 = "v_burst1_K=3000_Im.txt"; 
    readtxt2array(filename2, filelength, v_Im);

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
    int Lv = 6001;
    vector<double>  v_I(v_Re, v_Re + Lv); 
    vector<double>  v_Q(v_Im, v_Im + Lv); 

    int L_syncSig = 858;
    vector<double>  syncSig_I(syncSig_Re, syncSig_Re + L_syncSig); 
    vector<double>  syncSig_Q(syncSig_Im, syncSig_Im + L_syncSig);    

    ///////////////////////
    int K = 3000;
    double Ts_USRP = 1.6 * pow(10, -7); ;
    //double *cfo_hat;
    //double *cfo_hat = (double*)malloc(1 * sizeof(double)); //the memory pre-allocate for symbol mapping
    vector<double> CFOhat_PHOhat_d;
    CFOhat_PHOhat_d.reserve(3); //cfo_pho_hat = [pho_hat cfo_hat d]

    //////////////////////  CFO, PHO, TO estimation  //////////////////////
    //CFOPHOTO_estimator(v_I, v_Q, Lv, syncSig_I, syncSig_Q, L_syncSig, K, Ts_USRP);
    CFOPHOTO_estimator_test2(CFOhat_PHOhat_d, v_I, v_Q, Lv, syncSig_I, syncSig_Q, L_syncSig, K, Ts_USRP);
    double cfo_hat, pho_hat, d;
    cfo_hat = CFOhat_PHOhat_d[0]; 
    pho_hat = CFOhat_PHOhat_d[1]; 
    d = CFOhat_PHOhat_d[2];
    cout << cfo_hat << "\n";
    cout << pho_hat << "\n";
    cout << d << "\n";

    ////////////////////// Reget the correct received pilot //////////////////////  
/*
    // Get the exact pilot position
    int new_sps = 26;
    int start_burst = 11228;
    int exact_pilot_position = start_burst + d;
    int end_burst_original = start_burst + K;
    vector<double> RegetReceived_pilot_I, RegetReceived_pilot_Q;
    RegetReceived_pilot_I.reserve(L_syncSig); RegetReceived_pilot_Q.reserve(L_syncSig); 
    vector<double> Regetdata_burst_I, Regetdata_burst_Q;
    //get_databurst(start_burst, end_burst, Sig_I, Sig_Q, syncSig_I, syncSig_Q, exact_pilot_position, new_sps);

    int analysis_symbols_original = end_burst_original - start_burst;
    int new_end_burst;
    int analysis_symbols_modified;
    //number of analysis_symbols should be an integer multiple of sps
    if(analysis_symbols_original % new_sps != 0){
        new_end_burst = end_burst_original -  analysis_symbols_original % new_sps;
    }
    else{
        new_end_burst = end_burst_original;
    }
    analysis_symbols_modified = new_end_burst - start_burst;

    if(analysis_symbols_modified < L_syncSig){
        analysis_symbols_modified = L_syncSig;
        new_end_burst = start_burst + L_syncSig;
    }      
    int L_Sig = new_end_burst - start_burst;
    RegetReceived_pilot_I.reserve(L_Sig); RegetReceived_pilot_Q.reserve(L_Sig); 
    for (int i = start_burst; i <= new_end_burst - 1; i++){
        data_burst.push_back(Sig_I[i-1]);
        data_burst.push_back(Sig_Q[i-1]);
    }
*/
    //data_burst = Sig(start_burst : new_end_burst - 1); 

    //Received_pilot = data_burst(1:length(syncSig));

    //test_printf_2_vec(v_I, v_Q, 20);
    //test_printf_2_vec(syncSig_I, syncSig_Q, 20);
    
    return 0;
}