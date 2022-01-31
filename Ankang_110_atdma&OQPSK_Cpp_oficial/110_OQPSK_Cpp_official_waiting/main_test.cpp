#include <stdio.h>
#include <iostream>
#include<sys/time.h>
#include<unistd.h>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "DataConversion.h"
#include "DownConversion.h"
#include "SRRC.h"
#include "convolution.h"
#include "Upsample.h"
#include "Downsample_int.h"
#include "resample.h"
#include "variance_for_constellation.h"
#include "pilot.h"
#include "pilot_convolution.h"
#include "sum.h"
#include "sinc.h"
#include "MAX.h"
#include "MIN.h"
#include "find_high_pilot.h"
#include "abs_mean.h"
#include "point_divide_real.h"
#include "point_divide_imag.h"
#include "point_multiple_real.h"
#include "point_multiple_imag.h"
#include "pilot_compare.h"
#include "pilot_detection.h"
#include "phase_shift.h"
#include "frequency_shift.h"
#include "CPO_find.h"
#include "convert_to_index.h"
#include "Demodulate.h"
#include "QPSKor8PSK.h"
#include "matrix_write_file.h"
#include<bits/stdc++.h>
#include <omp.h>
#define pi 3.141592653589793238462643383279502884

using namespace std;

int main() 
{ 
    /******************** Inputted Data Paremeters ********************/
    double fc = -5459.6 * pow(10, 3); // center frequency of subcarrier
    // Tx info.
    int detection_type = 0 ;
    // detect_type = 0 for atdma_QPSK 
    // detect_type = 1 for atdma_8PSK 
    // detect_type = 2 for OQPSK 
    // detect_type = 3 for blind detection   
    double fsy = 1600 *pow( 10,3 ) ;        // symbol rate  
    double Tsy = 1/fsy;                     // symbol period
    double fs_USRP = 25 * pow(10, 6);     // sampling frequency
    char MOD_type[] = "QPSK" ;  
    int filelength = 1*pow(10, 7); // Number of used samples
    const char *filename ; 
    filename = "burst1_sr25.0_dtCplx_st8t"; //110 OQPSK
    const char *filename_save ; 
    filename_save = "n5459_symbol.txt"; //110 OQPSK
    int FilterSpanInSymbols = 3;
    float RolloffFactor = 0.3;
    /******************** Process Data Paremeters ********************/
    double Ts_USRP = 1/fs_USRP;             // sampling period
    double sps_USRP = fs_USRP/fsy;          // SamplePerSymbol. NOTE: USRP_sps is not interger. While sps should be even integer, so Resample Process is needed later on. 
    int IPOINT = ceil(sps_USRP) ;
    double resample_factor = IPOINT/sps_USRP;
    int new_sps = IPOINT;
    /*************************************Pilot Detection***************************************************/

    /********************          Read File          ********************/
    
    struct  timeval start_pilot_detection;
    struct  timeval end_pilot_detection;
    unsigned long diff_pilot_detection;
    gettimeofday(&start_pilot_detection,NULL); 


    int detect_filelength = 2000000;//detect 1.5e6 data samples
    vector<int> rawDataI_pilot_detection(detect_filelength/2);//int *rawDataI = (int*)malloc(filelength/2 * sizeof(int)); //int rawDataI[filelength/2]; // cannot work
    vector<int> rawDataQ_pilot_detection(detect_filelength/2);//int *rawDataI = (int*)malloc(filelength/2 * sizeof(int)); //int rawDataI[filelength/2]; // cannot work
    DataConversion(filename, detect_filelength, rawDataI_pilot_detection, rawDataQ_pilot_detection); 
    /********************       Down Conversion        ********************/
    vector<double> DataDownconversion_pilot_detection_I(detect_filelength/2);//double *DataDownconversion_I = (double*)malloc(filelength/2 * sizeof(double));
    vector<double> DataDownconversion_pilot_detection_Q(detect_filelength/2);//double *DataDownconversion_Q = (double*)malloc(filelength/2 * sizeof(double));
    DownConversion(rawDataI_pilot_detection, rawDataQ_pilot_detection, DataDownconversion_pilot_detection_I, DataDownconversion_pilot_detection_Q, detect_filelength/2, fc, fs_USRP);
    // free memory
    rawDataI_pilot_detection.clear();
    rawDataI_pilot_detection.shrink_to_fit();
    rawDataQ_pilot_detection.clear();
    rawDataQ_pilot_detection.shrink_to_fit();
    /********************     Pulse Shaping Filter    ********************/
   

    int FilterLength_pilot_detection = FilterSpanInSymbols * new_sps + 1;
    vector<double> PulseshapingFilter_pilot_detection(FilterLength_pilot_detection);
    SRRC(PulseshapingFilter_pilot_detection, FilterSpanInSymbols, RolloffFactor, new_sps, FilterLength_pilot_detection);
    

    /********************     Signal Passes through Filter    ********************/  
      
    vector<double> rsmooth_pilot_detection_I(DataDownconversion_pilot_detection_I.size());
    vector<double> rsmooth_pilot_detection_Q(DataDownconversion_pilot_detection_Q.size()); 
    convolution(DataDownconversion_pilot_detection_Q,PulseshapingFilter_pilot_detection,rsmooth_pilot_detection_Q);
    convolution(DataDownconversion_pilot_detection_I,PulseshapingFilter_pilot_detection,rsmooth_pilot_detection_I);   

    double abs_mean_rsmooth_pilot_detection = abs_mean(rsmooth_pilot_detection_I, rsmooth_pilot_detection_Q) ; 
    for( int i =0 ; i< rsmooth_pilot_detection_I.size() ; i++){
        rsmooth_pilot_detection_I[i]= rsmooth_pilot_detection_I[i]/abs_mean_rsmooth_pilot_detection ;
        rsmooth_pilot_detection_Q[i]= rsmooth_pilot_detection_Q[i]/abs_mean_rsmooth_pilot_detection;
    }   

    // free memory
    DataDownconversion_pilot_detection_I.clear();
    DataDownconversion_pilot_detection_I.shrink_to_fit();
    DataDownconversion_pilot_detection_Q.clear();
    DataDownconversion_pilot_detection_Q.shrink_to_fit();

    /********************     Resample    ********************/   
    vector<double> resample_signal_pilot_detection_I(ceil(rsmooth_pilot_detection_I.size()*resample_factor ) );
    vector<double> resample_signal_pilot_detection_Q(ceil(rsmooth_pilot_detection_Q.size()*resample_factor ) ); 
    resample( rsmooth_pilot_detection_I , rsmooth_pilot_detection_Q , resample_signal_pilot_detection_I , resample_signal_pilot_detection_Q , resample_factor , 2 );
        
    // free memory
    rsmooth_pilot_detection_I.clear();
    rsmooth_pilot_detection_I.shrink_to_fit();
    rsmooth_pilot_detection_Q.clear();
    rsmooth_pilot_detection_Q.shrink_to_fit();
    // normalize
    double abs_mean_resample_pilot_detection = abs_mean(resample_signal_pilot_detection_I , resample_signal_pilot_detection_Q ) ; 
    for ( int i ; i < resample_signal_pilot_detection_I.size() ; i++ ){
        resample_signal_pilot_detection_Q[i] = resample_signal_pilot_detection_Q[i] / abs_mean_resample_pilot_detection;
        resample_signal_pilot_detection_I[i] = resample_signal_pilot_detection_I[i] / abs_mean_resample_pilot_detection;
    }
    /********************     Map pilot    ********************/      
    vector<double> pilot_r;
    vector<double> pilot_i;
    vector<int> pilot_string;
    vector<double> inpilot_r;
    vector<double> inpilot_i;
    int space_type = 0 ;
    char data_type ;


    data_type = pilot_detection( resample_signal_pilot_detection_I, resample_signal_pilot_detection_Q, IPOINT , detection_type);
    printf("Pilot type is ");
    cout<< data_type<< endl;


    pilot(data_type , pilot_r, pilot_i, pilot_string, inpilot_r, inpilot_i, space_type);
    
    vector<double> inpilot_r_UP(inpilot_r.size()*IPOINT);
    vector<double> inpilot_i_UP(inpilot_i.size()*IPOINT);
    Upsample(inpilot_r, inpilot_r_UP, IPOINT);
    Upsample(inpilot_r, inpilot_r_UP, IPOINT);


    gettimeofday(&end_pilot_detection,NULL);
    diff_pilot_detection = 1000000 * (end_pilot_detection.tv_sec-start_pilot_detection.tv_sec)+ end_pilot_detection.tv_usec-start_pilot_detection.tv_usec;     
    double diff_pilot_detection_result = diff_pilot_detection/1000000.0 ;
    //printf("PIlot type is found in %f seconds!!!!!!!!!!\n",diff_pilot_detection_result);    


    /*************************************Start demodulate data***************************************************/


    /********************          Read File          ********************/
    vector<int> rawDataI(filelength/2);//int *rawDataI = (int*)malloc(filelength/2 * sizeof(int)); //int rawDataI[filelength/2]; // cannot work
    vector<int> rawDataQ(filelength/2);//int *rawDataI = (int*)malloc(filelength/2 * sizeof(int)); //int rawDataI[filelength/2]; // cannot work


    struct  timeval start_data_conversion;
    struct  timeval end_data_conversion;
    unsigned long diff_data_conversion;
    gettimeofday(&start_data_conversion,NULL);
	DataConversion(filename, filelength, rawDataI, rawDataQ);


    gettimeofday(&end_data_conversion,NULL);
    diff_data_conversion = 1000000 * (end_data_conversion.tv_sec-start_data_conversion.tv_sec)+ end_data_conversion.tv_usec-start_data_conversion.tv_usec;     
    double diff_data_conversion_result = diff_data_conversion/1000000.0 ;

	//printf("Data conversion is done in %f seconds!!!!!!!!!!\n", diff_data_conversion_result );

    /********************       Down Conversion        ********************/
    vector<double> DataDownconversion_I(filelength/2);//double *DataDownconversion_I = (double*)malloc(filelength/2 * sizeof(double));
    vector<double> DataDownconversion_Q(filelength/2);//double *DataDownconversion_Q = (double*)malloc(filelength/2 * sizeof(double));


    struct  timeval start_down_conversion;
    struct  timeval end_down_conversion;
    unsigned long diff_down_conversion;
    gettimeofday(&start_down_conversion,NULL);

    DownConversion(rawDataI, rawDataQ, DataDownconversion_I, DataDownconversion_Q, filelength/2, fc, fs_USRP);

    gettimeofday(&end_down_conversion,NULL);
    diff_down_conversion = 1000000 * (end_down_conversion.tv_sec-start_down_conversion.tv_sec)+ end_down_conversion.tv_usec-start_down_conversion.tv_usec;     
    double diff_down_conversion_result = diff_down_conversion/1000000.0 ;

   
    printf("Down conversion is done in %f seconds!!!!!!!!!!\n", diff_down_conversion_result );
    
    
    // free memory

    rawDataI.clear();
    rawDataI.shrink_to_fit();
    rawDataQ.clear();
    rawDataQ.shrink_to_fit();
    
    /********************     Pulse Shaping Filter    ********************/
   
    struct  timeval start_SRRC;
    struct  timeval end_SRRC;
    unsigned long diff_SRRC;
    gettimeofday(&start_SRRC,NULL); 
    
    
    

    int FilterLength = FilterSpanInSymbols * new_sps + 1;
	vector<double> PulseshapingFilter(FilterLength);
	SRRC(PulseshapingFilter, FilterSpanInSymbols, RolloffFactor, new_sps, FilterLength);
	

    /********************     Signal Passes through Filter    ********************/  
	  
    vector<double> rsmooth_I(DataDownconversion_I.size());
    vector<double> rsmooth_Q(DataDownconversion_Q.size()); 
    convolution(DataDownconversion_Q,PulseshapingFilter,rsmooth_Q);
    convolution(DataDownconversion_I,PulseshapingFilter,rsmooth_I);
    

    double abs_mean_rsmooth = abs_mean(rsmooth_I, rsmooth_Q) ; 
    for( int i =0 ; i< rsmooth_I.size() ; i++){
        rsmooth_I[i]= rsmooth_I[i]/abs_mean_rsmooth ;
        rsmooth_Q[i]= rsmooth_Q[i]/abs_mean_rsmooth ;
    }   

    // free memory
    DataDownconversion_I.clear();
	DataDownconversion_I.shrink_to_fit();
    DataDownconversion_Q.clear();
	DataDownconversion_Q.shrink_to_fit();

    gettimeofday(&end_SRRC,NULL);
    diff_SRRC =  1000000 * (end_SRRC.tv_sec-start_SRRC.tv_sec)+end_SRRC.tv_usec-start_SRRC.tv_usec;//     
    double diff_SRRC_result = diff_SRRC/1000000.0 ;
	
    printf("SRRC is done in %f seconds!!!!!!!!!!\n", diff_SRRC_result);
    
    /********************     Resample    ********************/   
    struct  timeval start_Resample;
    struct  timeval end_Resample;
    unsigned long diff_Resample;
    gettimeofday(&start_Resample,NULL); 
    
    
    
    vector<double> resample_signal_I(ceil(rsmooth_I.size()*resample_factor ) );
    vector<double> resample_signal_Q(ceil(rsmooth_Q.size()*resample_factor ) ); 
    resample( rsmooth_I , rsmooth_Q , resample_signal_I , resample_signal_Q , resample_factor , 0 );
    
    
    
    // free memory
    rsmooth_I.clear();
	rsmooth_I.shrink_to_fit();
    rsmooth_Q.clear();
	rsmooth_Q.shrink_to_fit();
    // normalize
    double abs_mean_resample = abs_mean(resample_signal_I , resample_signal_Q ) ; 
    for ( int i ; i < resample_signal_I.size() ; i++ ){
        resample_signal_Q[i] = resample_signal_Q[i] / abs_mean_resample;
        resample_signal_I[i] = resample_signal_I[i] / abs_mean_resample;
    }
    
    gettimeofday(&end_Resample,NULL);
    diff_Resample = 1000000 * (end_Resample.tv_sec-start_Resample.tv_sec)+ end_Resample.tv_usec-start_Resample.tv_usec;     
    double diff_Resample_result = diff_Resample/1000000.0 ;

    printf("Resampling is done in %f seconds!!!!!!!!!!\n", diff_Resample_result );


    /********************     MF    ********************/ 
    struct  timeval start_MF;
    struct  timeval end_MF;
    unsigned long diff_MF;
    gettimeofday(&start_MF,NULL); 
    
        
	vector<double> MF(resample_signal_I.size()) ;
    vector<double> MF_conv_1(resample_signal_I.size()) ;
    vector<double> MF_conv_2(resample_signal_I.size()) ;
    vector<double> MF_conv_3(resample_signal_I.size()) ;
    vector<double> MF_conv_4(resample_signal_I.size()) ;
	
    pilot_convolution(resample_signal_I , inpilot_r_UP , MF_conv_1, space_type, IPOINT) ;
    pilot_convolution(resample_signal_Q , inpilot_i_UP , MF_conv_2, space_type, IPOINT) ;
    pilot_convolution(resample_signal_I , inpilot_i_UP , MF_conv_3, space_type, IPOINT) ;
    pilot_convolution(resample_signal_Q , inpilot_r_UP , MF_conv_4, space_type, IPOINT) ;
    
    
        //#pragma omp parallel for
    for (int i = 0 ; i < MF.size() ; i++ ){ // conv( s_r + 1j* s_i , conj( p_r , 1j* p_i )   ) = conv(s_r,p_r)+conv(s_i,p_i) -1j*conv(s_r,p_i)+1j*conv(s_i,p_r)
        MF[i] = sqrt( pow( MF_conv_1[i] + MF_conv_2[i] , 2 ) + pow( -MF_conv_3[i] + MF_conv_4[i]  , 2 ) );
    }
    // free memory 
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
    
 //    printf("Pilot position is \n"); 
 //    for( int i =0 ; i< pilot_position.size() ; i++){
 //        cout<< pilot_position[i] << " " ;
 //    }
	// cout<< endl ;


    gettimeofday(&end_MF,NULL);
    diff_MF = 1000000 * (end_MF.tv_sec-start_MF.tv_sec)+ end_MF.tv_usec-start_MF.tv_usec;     
    double diff_MF_result = diff_MF/1000000.0 ;
	printf("MF is found in %f seconds!!!!!!!!!!\n",diff_MF_result);    

    MF.clear();
    MF.shrink_to_fit();
    
    /********************     Demodulation    ********************/ 
    struct  timeval start_demod;
    struct  timeval end_demod;
    unsigned long diff_demod;
    gettimeofday(&start_demod,NULL); 
    
    int burst_num = pilot_position.size() ;
    int point_num_burst = inpilot_i.size();
    int num_burst = inpilot_i.size()/30 ;
    vector<vector<double>> processed_data_recieved_I( burst_num , vector<double> (point_num_burst));
    vector<vector<double>> processed_data_recieved_Q( burst_num , vector<double> (point_num_burst));
    vector<vector<int>> processed_symbol_recieved( burst_num , vector<int> (point_num_burst, 0));
    //vector<double> processed_data_recieved_I(burst_num , point_num_burst) ;
    //vector<double> processed_data_recieved_Q(burst_num , point_num_burst) ;
    //vector<int> processed_symbol_recieved(burst_num , point_num_burst) ;
    #pragma omp parallel for
    for( int demod_index = 0 ; demod_index < burst_num ; demod_index++ ){
    	//cout<< endl << "burst index is " << demod_index << "\n" ;
        Demodulate( pilot_position , resample_signal_I , resample_signal_Q , point_num_burst , num_burst , IPOINT ,  fs_USRP*resample_factor, pilot_r, pilot_i, inpilot_r, inpilot_i ,MOD_type , processed_data_recieved_I , processed_data_recieved_Q , processed_symbol_recieved , demod_index);
	}

    gettimeofday(&end_demod,NULL);
    diff_demod = 1000000 * (end_demod.tv_sec-start_demod.tv_sec)+ end_demod.tv_usec-start_demod.tv_usec;     
    double diff_demod_result = diff_demod/1000000.0 ;
	printf("Demodulation is done in %f seconds!!!!!!!!!!\n", diff_demod_result );

    // for( int demod_index = 0 ; demod_index < burst_num ; demod_index++ ){
    //     cout<< endl << "burst index is " << demod_index << "\n" ;
    //         for( int data_index = 0 ; data_index < point_num_burst ; data_index++ ){        
    //             cout << processed_symbol_recieved[ demod_index ][ data_index ] ;
    //         }   
    // }    
    printf("\n\n\nTotal time is %f seconds\n", diff_MF_result + diff_Resample_result + diff_SRRC_result + diff_down_conversion_result + diff_demod_result );

    matrix_write_file( processed_symbol_recieved, burst_num , point_num_burst , filename_save);

    return 0;

}
