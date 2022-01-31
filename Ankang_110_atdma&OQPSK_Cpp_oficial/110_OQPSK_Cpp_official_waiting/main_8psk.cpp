// Execute instruction:
// (1) g++ -o d main_adaptive.cpp -lfftw3 -lm -std=c++17
// (2) ./d

#include <stdio.h>
#include <iostream>
#include<sys/time.h>
#include<unistd.h>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define pi 3.141592653589793238462643383279502884

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
#include "phase_shift.h"
#include "frequency_shift.h"
#include "CPO_find.h"
#include "convert_to_index.h"
#include "Demodulate.h"
#include "QPSKor8PSK.h"
#include "matrix_write_file.h"
#include<bits/stdc++.h>
#include <omp.h>
using namespace std;
#include <string> 
#include "str2double.h"
#include "str2float.h"
#include "str2int.h"
#include "str2pilot.h"
#include "find_num_ind.h"
#include "QPSKor8PSK.h"
#include "pilot_detection_QPSK.h"
#include "pilot_detection_8PSK.h"


int main() 
{ 
    /******************** Inputted Data Paremeters ********************/
    int resample_point ;
    int detect_type  ;    
    int FilterSpanInSymbols ;
    int filelength ; // Number of used samples    

    double fc ; // center frequency of subcarrier
    // detect_type = 0 for atdma_QPSK 
    // detect_type = 1 for atdma_8PSK 
    // detect_type = 2 for OQPSK 
    // detect_type = 3 for blind detection   
    double fsy ;        // symbol rate  
    double fs_USRP ;     // sampling frequency
    float RolloffFactor; 
    char *MOD_type ; 
    //MOD_type = "QPSK" ;  
    //char *file_read ; 
    filename = "burst1_sr25.0_dtCplx_st8t"; //110 OQPSK
    char *file_save ; 
    //filename_save = "n5459_symbol.txt"; //110 OQPSK
    char pilot_type = 'A' ;

    /******************** Read Parameter *****************************/




    int point_position = 0 ;
    int start_index = 0 ;
    int end_index = 0 ;
    string data;
    int test_type_pilot = 0 ;
    int pilot_switch = 0 ;
    int space_type = 0 ;
    char data_type ;
    // vector<double> pilot_input_I ;
    // vector<double> pilot_input_Q  ;
    // vector<double> inpilot_input_I ;
    // vector<double> inpilot_input_Q ;
    // vector<int> pilot_string ;



        vector<double> pilot_r;
        vector<double> pilot_i;
        vector<int> pilot_string;
        vector<double> inpilot_r;
        vector<double> inpilot_i;





     














    /******************** Process Data Paremeters ********************/
    double Ts_USRP = 1/fs_USRP;             // sampling period
    double sps_USRP = fs_USRP/fsy;          // SamplePerSymbol. NOTE: USRP_sps is not interger. While sps should be even integer, so Resample Process is needed later on. 
    int IPOINT = ceil(sps_USRP) ;
    double resample_factor = IPOINT/sps_USRP;
    int new_sps = IPOINT;
    double Tsy = 1/fsy;  


    const char* test_MOD;
    test_MOD = MOD_type ;

    int data_para = 0 ;
    if (!strcmp(test_MOD , "OQPSK" )){
        data_para = 1 ;
        pilot_type = 'U' ;
    }

    const char* filename;
    filename = file_read ;

    //cout << filename << endl << "burst1_sr25.0_dtCplx_st8t" << endl ;
    const char* filename_save;
    filename_save = file_save ;





        /********************     Map pilot    ********************/      
        vector<double> pilot_r;
        vector<double> pilot_i;
        vector<int> pilot_string;
        vector<double> inpilot_r;
        vector<double> inpilot_i;

        pilot(pilot_type , pilot_r, pilot_i, pilot_string, inpilot_r, inpilot_i, space_type);


    








        vector<double> inpilot_r_UP( inpilot_i.size()*IPOINT);
        vector<double> inpilot_i_UP( inpilot_i.size()*IPOINT);
        Upsample( inpilot_r , inpilot_r_UP, IPOINT);
        Upsample( inpilot_i , inpilot_i_UP, IPOINT);



    /*************************************Start demodulate data***************************************************/
    if (pilot_type <= 'P' || data_para == 0){

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
        resample( rsmooth_I , rsmooth_Q , resample_signal_I , resample_signal_Q , resample_factor , resample_point );
        
        
        
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
        vector<vector<int>> processed_symbol_recieved( burst_num , vector<int> (point_num_burst));
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



        /////////////////////////  SNR estimation  /////////////////////////
        // only use pilots to calculate the SNR

        string DataType;
        if ( data_type <= 'H'){
            string DataType( "QPSK" ) ;
        }
        else{
            string DataType( "QPSK" ) ;
        }

        vector<double> vec_b_i_hat_p_K( burst_num * point_num_burst ) ;
        vector<double> vec_b_q_hat_p_K( burst_num * point_num_burst ) ;
        vector<double> ReceivedSym_p_K( burst_num * pilot_string.size() ) ;


        for( int i = 0 ; i < burst_num ; i++ ){
            for( int j = 0 ; j < point_num_burst ; j++ ){
                vec_b_i_hat_p_K[ i * point_num_burst + j ] =  processed_data_recieved_I[i][j];
                vec_b_q_hat_p_K[ i * point_num_burst + j ] =  processed_data_recieved_Q[i][j];
            }
        }


        for( int i = 0 ; i < burst_num ; i++){
            for( int  j = 0 ; j < pilot_string.size() / ( space_type + 2 )  ; j++){
                for( int k = 0 ; k < space_type + 2 ; k++){
                    ReceivedSym_p_K[  i*pilot_string.size() + j*( space_type + 2 ) + k ] = (double)processed_symbol_recieved[i][ j*30 + k] ;
                }
            }
        }








        double SNR_dB;
        SNR_dB = SNRestimation(DataType, vec_b_i_hat_p_K, vec_b_q_hat_p_K);
        vec_b_i_hat_p_K.clear(); vec_b_q_hat_p_K.clear(); 
        //printf("SNR estimation is done.\n");
        //printf("SNR of the signal is %f.\n", SNR_dB);

        /////////////////////////  SER and BER estimation  /////////////////////////

        vector<double> pilot_string_double(pilot_string.size());

        for( int i = 0 ; i < pilot_string.size() ; i++){
            pilot_string_double[i] = (double)pilot_string[i] ; 
        } 


        vector<double> SER_BER_errorpilot;
        SER_BER_errorpilot.reserve(3);
        ErrorRateCalculation(SER_BER_errorpilot, DataType, pilot_string_double, ReceivedSym_p_K);
        ReceivedSym_p_K.clear();
        double SER, BER, errorpilot;
        SER = SER_BER_errorpilot[0];
        BER = SER_BER_errorpilot[1];
        errorpilot = SER_BER_errorpilot[2];






















    }

    return 0;

}
