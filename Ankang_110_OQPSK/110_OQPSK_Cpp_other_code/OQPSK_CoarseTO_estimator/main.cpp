#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <iostream>
using namespace std;

#include "readtxt2array.h"
#include "norm2.h"
#include "norm2_vec.h"

#include "test_printf_2_vec.h"
#include "test_printf_1_vec.h"
#include "coarseTO_estimator_test.h"

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

    /********************          Read input file(from matlab)         ********************/
    int filelength = 532480;
    double *arraySig_I = (double*)malloc(filelength * sizeof(double)); 
    const char *filename1;
    filename1 = "Sig_Re.txt"; 
    readtxt2array(filename1, filelength, arraySig_I);

    filelength = 532480;
    double *arraySig_Q = (double*)malloc(filelength * sizeof(double)); 
    const char *filename2;
    filename2 = "Sig_Im.txt"; 
    readtxt2array(filename2, filelength, arraySig_Q);

    filelength = 858;
    double *arraySyncSig_I = (double*)malloc(filelength * sizeof(double)); 
    const char *filename3;
    filename3 = "syncSig_Re.txt"; 
    readtxt2array(filename3, filelength, arraySyncSig_I);

    filelength = 858;
    double *arraySyncSig_Q = (double*)malloc(filelength * sizeof(double)); 
    const char *filename4;
    filename4 = "syncSig_Im.txt"; 
    readtxt2array(filename4, filelength, arraySyncSig_Q);

    /********************    Read output result of complex convolution (from matlab)   ********************/
    
    filelength = 532480;
    double *y1_Re = (double*)malloc(filelength * sizeof(double)); 
    const char *filename5;
    filename5 = "y1_Re.txt"; 
    readtxt2array(filename5, filelength, y1_Re);
    
    filelength = 532480;
    double *y1_Im = (double*)malloc(filelength * sizeof(double)); 
    const char *filename6;
    filename6 = "y1_Im.txt"; 
    readtxt2array(filename6, filelength, y1_Im);
    
    //////////////////////    Change array data type to vector   //////////////////////  
    int SigLength_afterResample = 532480;
    vector<double>  Sig_I(arraySig_I, arraySig_I + SigLength_afterResample); //, Sig_Q;
    vector<double>  Sig_Q(arraySig_Q, arraySig_Q + SigLength_afterResample); //, Sig_Q;

    int L_syncSig = 858;
    vector<double>  syncSig_I(arraySyncSig_I, arraySyncSig_I + L_syncSig); //, Sig_Q;
    vector<double>  syncSig_Q(arraySyncSig_Q, arraySyncSig_Q + L_syncSig); //, Sig_Q;
 
     int Ly_matlab = 532480;
    vector<double>  y1_I_matlab(y1_Re, y1_Re + Ly_matlab); //, Sig_Q;
    vector<double>  y1_Q_matlab(y1_Im, y1_Im + Ly_matlab); //, Sig_Q;
     

    //////////////////////   Course TO estimation   //////////////////////  

    vector<double>  y1_I, y1_Q;
    vector<int> coarse_pilot_position;

    y1_I.reserve(SigLength_afterResample); y1_Q.reserve(SigLength_afterResample);
    coarseTO_estimator_test(Sig_I, Sig_Q, SigLength_afterResample, syncSig_I, syncSig_Q, L_syncSig ,new_sps, y1_I, y1_Q, coarse_pilot_position);
    
    int Ly;
    Ly = y1_I.size();
    
    //norm2_vec(y1_I, y1_Q, y1_I_matlab, y1_Q_matlab, Ly);
    
    //test_printf_2_vec(y1_I, y1_Q, 10); 
    //test_printf_2_vec(y1_I_matlab, y1_Q_matlab, 10);

    //test_printf_2_vec(syncSig_I, syncSig_Q, 10);


/*
    int L_regetpilot = 858;
    vector<double> RegetReceived_pilot_I, RegetReceived_pilot_Q;
    RegetReceived_pilot_I.reserve(L_regetpilot); RegetReceived_pilot_Q.reserve(L_regetpilot);
    arraytovec(RegetReceived_pilot_I, RegetReceived_pilot_Q, array1, array2, L_regetpilot); 
    free(array1); free(array2);
    */



    //test_printf_1_vec(Sig_I, 10);

    //cout << b_i_hat.size() << '\n';
    return 0;
}
