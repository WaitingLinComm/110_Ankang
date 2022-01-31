#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "DataConversion.h"
#include "DownConversion.h"
#include "SRRC.h"
#include "convolution.h"
#include "Upsample.h"
#include "resample.h"
#include "variance_for_constellation.h"
#include "pilot.h"
#include "sum.h"
#include "MAX.h"
#include "find_high_pilot.h"
#include "abs_mean.h"
using namespace std;

int main() 
{ 
    /******************** Data Paremeters ********************/
    double fc = -5459.6 * pow(10, 3); // center frequency of subcarrier
    // Tx info.
    double fsy = 1600 *pow( 10,3 ) ;        // symbol rate  
    double Tsy = 1/fsy;                     // symbol period
    // USRP info.
    double fs_USRP = 25 * pow(10, 6);     // sampling frequency
    double Ts_USRP = 1/fs_USRP;             // sampling period
    double sps_USRP = fs_USRP/fsy;          // SamplePerSymbol. NOTE: USRP_sps is not interger. While sps should be even integer, so Resample Process is needed later on. 
    int IPOINT = ceil(sps_USRP) ;
    double resample_factor = IPOINT/sps_USRP;
    /********************          Read File          ********************/
    const char *filename;
    filename = "burst1_sr25.0_dtCplx_st8t"; //110 OQPSK
    int filelength = pow(10, 6); // Number of used samples 
 
    vector<int> rawdataI(filelength/2);//int *rawDataI = (int*)malloc(filelength/2 * sizeof(int)); //int rawDataI[filelength/2]; // cannot work
    vector<int> rawdataQ(filelength/2);//int *rawDataI = (int*)malloc(filelength/2 * sizeof(int)); //int rawDataI[filelength/2]; // cannot work
    
    DataConversion(filename, filelength, rawDataI, rawDataQ);
    printf("Data conversion is done!!!!!!!!!!\n");

    /********************       Down Conversion        ********************/
    vector<double> DataDownconversion_I(filelength/2);//double *DataDownconversion_I = (double*)malloc(filelength/2 * sizeof(double));
    vector<double> DataDownconversion_Q(filelength/2);//double *DataDownconversion_Q = (double*)malloc(filelength/2 * sizeof(double));

    DownConversion(rawdataI, rawdataQ, DataDownconversion_I, DataDownconversion_Q, filelength/2, fc, fs_USRP);

    printf("Down conversion  is done!!!!!!!!!!\n");
    // free memory
    rawdataI.clear();
	rawdataI.shrink_to_fit();
    rawdataQ.clear();
	rawdataQ.shrink_to_fit();
    
    /********************     Pulse Shaping Filter    ********************/
    int new_sps = 26;
    int FilterSpanInSymbols = 6;
    float RolloffFactor = 0.3;
    int FilterLength = FilterSpanInSymbols * new_sps + 1;
	vector<double> PulseshapingFilter(FilterLength);
	SRRC(PulseshapingFilter, FilterSpanInSymbols, RolloffFactor, new_sps, FilterLength);


    /********************     Signal Passes through Filter    ********************/    
    vector<double> rsmooth_I(DataDownconversion_I.size());
    vector<double> rsmooth_Q(DataDownconversion_Q.size()); 
    convolution(DataDownconversion_Q,PulseshapingFilter,rsmooth_Q);
    convolution(DataDownconversion_I,PulseshapingFilter,rsmooth_I);
    // free memory
    DataDownconversion_I.clear();
	DataDownconversion_I.shrink_to_fit();
    DataDownconversion_Q.clear();
	DataDownconversion_Q.shrink_to_fit();

    printf("SRRC is done!!!!!!!!!!\n");

    /********************     Resample    ********************/   
    vector<double> resample_signal_I(rsmooth_I.size());
    vector<double> resample_signal_Q(rsmooth_Q.size()); 
    resample( DataDownconversion_I , DataDownconversion_Q , resample_signal_I , resample_signal_Q ,resample_factor , 2 );
    // free memory
    DataDownconversion_I.clear();
	DataDownconversion_I.shrink_to_fit();
    DataDownconversion_Q.clear();
	DataDownconversion_Q.shrink_to_fit();
    // normalize
    double abs_mean_resample = abs_mean(resample_signal_I , resample_signal_Q ) ; 
    for ( int i ; i < resample_signal_I.size() ; i++ ){
        resample_signal_Q[i] = resample_signal_Q[i] / abs_mean_resample;
        resample_signal_I[i] = resample_signal_I[i] / abs_mean_resample;
    }

    printf("Resampling is done!!!!!!!!!!\n"); 
    /********************     Map pilot    ********************/ 
    vector<double> pilot_r;
    vector<double> pilot_i;
    vector<int> pilot_string;
    vector<double> inpilot_r;
    vector<double> inpilot_i;


    pilot('C' , pilot_r, pilot_i, pilot_string, inpilot_r, inpilot_i);
    /********************     MF    ********************/ 
    vector<double> MF(resample_signal_I.size()) ;
    vector<double> MF_real(resample_signal_I.size()) ;
    vector<double> MF_imag(resample_signal_I.size()) ;
    vector<double> MF_conv_1(resample_signal_I.size()) ;
    vector<double> MF_conv_2(resample_signal_I.size()) ;
    vector<double> MF_conv_3(resample_signal_I.size()) ;
    vector<double> MF_conv_4(resample_signal_I.size()) ;

    convolution(resample_signal_I , inpilot_r , MF_conv_1) ;
    convolution(resample_signal_Q , inpilot_i , MF_conv_2) ;
    convolution(resample_signal_I , inpilot_i , MF_conv_3) ;
    convolution(resample_signal_Q , inpilot_r , MF_conv_4) ;

    for (int i = 0 ; i < MF_real.size() ; i++ ){// conv( s_r + 1j* s_i , conj( p_r , 1j* p_i )   ) = conv(s_r,p_r)+conv(s_i,p_i) -1j*conv(s_r,p_i)+1j*conv(s_i,p_r)
        MF_real[i] = MF_conv_1[i] + MF_conv_2[i] ;
        MF_imag[i] = -MF_conv_3[i] + MF_conv_4[i] ;
        MF[i] = sqrt( pow( MF_real[i] , 2 ) + pow( MF_imag[i] , 2 ) );
    }  
    // free memory
    MF_real.clear();
	MF_real.shrink_to_fit();    
    MF_imag.clear();
	MF_imag.shrink_to_fit();
    MF_conv_1.clear();
	MF_conv_1.shrink_to_fit(); 
    MF_conv_2.clear();
	MF_conv_2.shrink_to_fit(); 
    MF_conv_3.clear();
	MF_conv_3.shrink_to_fit(); 
    MF_conv_4.clear();
	MF_conv_4.shrink_to_fit();    

    /********************     Find pilot    ********************/ 
    vector<int> pilot_position;
    find_high_pilot( pilot_position , MF, IPOINT) ;

    for( int i =0 ; i< pilot_position.size() ; i++){
        cout<< pilot_position[i] << '\n' ;
    }













    return 0;
}
