// Execute instruction:
// (1) g++ -o d main_adaptive.cpp -lfftw3 -lm -std=c++17 -fopenmp
// (2) ./d

#include <fftw3.h>
#include <stdio.h>
#include <iostream>
//#include<sys/time.h>
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
//#include<bits/stdc++.h>
//#include <omp.h>
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



#include "OQPSK_DataConversion.h"                 // used in "OQPSK_main"
#include "OQPSK_DC_Blocker.h"                     // used in "OQPSK_main"
#include "OQPSK_DownConversion.h"                 // used in "OQPSK_main"
#include "OQPSK_complex_convolution.h"            // used in "OQPSK_main and "OQPSK_frontendTx.h"
#include "OQPSK_overlap_add.h"                    // used in "OQPSK_coarseTO_estimator.h"
#include "OQPSK_Filter_TransferFunction.h"        // used in "OQPSK_main" 
#include "OQPSK_sinc.h"                           // used in "OQPSK_resample.h"
#include "OQPSK_resample.h"                       // used in "OQPSK_main"
#include "OQPSK_SRRC.h"                           // used in "OQPSK_main"
#include "OQPSK_symbol_mapping.h"                 // used in "OQPSK_main"

#include "OQPSK_bitReorganization.h"              // used in "OQPSK_frontendTx.h"
#include "OQPSK_bitMapping.h"                     // used in "OQPSK_frontendTx.h"
#include "OQPSK_I_Q_interlace.h"                  // used in "OQPSK_frontendTx.h"
#include "OQPSK_Upsample.h"                       // used in "OQPSK_frontendTx.h"
#include "OQPSK_frontendTx.h"                     // used in "OQPSK_main"

#include "OQPSK_sum_vec.h"                        // used in "OQPSK_find_high_pilot.h"
#include "OQPSK_MAX_vec.h"                        // used in "OQPSK_find_high_pilot.h"
#include "OQPSK_find_high_pilot.h"                // used in "OQPSK_coarseTO_estimator.h"
#include "OQPSK_Flip_vec.h"                       // used in "OQPSK_coarseTO_estimator.h"
#include "OQPSK_complex_convolution_with_norm.h"  // used in "OQPSK_coarseTO_estimator.h"
#include "OQPSK_find_burst_interval.h"            // used in "coarseTO_estimator_v3.h"
#include "OQPSK_coarseTO_estimator.h"             // used in "OQPSK_main"
#include "OQPSK_coarseTO_estimator_v2.h"                // used in main

#include "OQPSK_tmp_cfo_compen.h"                 // used in "OQPSK_CFOPHOTO_estimator_v2.h"
#include "OQPSK_CFOPHOTO_estimator.h"             // used in "OQPSK_main"
#include "OQPSK_CFOPHOTO_estimator_v2.h"          // used in "OQPSK_main"

#include "OQPSK_get_databurst.h"                  // used in "OQPSK_main"
#include "OQPSK_CFOPHOcompen.h"                   // used in "OQPSK_main"
#include "OQPSK_conv_vec.h"                       // used in "OQPSK_OQPSK_aftendRx.h"
#include "OQPSK_aftendRx.h"                       // used in "OQPSK_main"
#include "OQPSK_DecisionProcess.h"                // used in "OQPSK_main"
#include "OQPSK_bitReorganizationRx.h"            // used in "OQPSK_main"
#include "OQPSK_symbol_demapping.h"               // used in "OQPSK_main"

#include "SNRestimation.h"                        // used in "OQPSK_main"
#include "ErrorRateCalculation.h"                 // used in "OQPSK_main"

#include "OQPSK_main_2.h"                           // used in main
#include "OQPSK_main_3.h"                           // used in main
#include "OQPSK_main_4.h"                           // used in main
#include "OQPSK_main_5.h"                           // used in main
#include "OQPSK_pilot_avg_power.h"

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
    char *file_read ; 
    //filename = "burst1_sr25.0_dtCplx_st8t"; //110 OQPSK
    char *file_save ; 
    //filename_save = "n5459_symbol.txt"; //110 OQPSK
    char pilot_type = 'A' ;

    /******************** Read Parameter *****************************/

    ifstream file;
    file.open("data.txt");
    int para_count = 0 ;

    string line;



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



    while( getline(file , line) ){
 





     



        find_num_ind( line , start_index , end_index , point_position );

        data = line.substr( start_index , end_index - start_index + 1 ) ;
        // for ( int i = 0 ; i < data.size() ; i++ ){
        //     cout<<data[i] ;
        // }
        // cout<< "\n" ;
        if ( para_count == 0 ){
            str2double( fsy , data , end_index - start_index  , point_position  ) ;
        }
        else if ( para_count == 1){
            str2double( fs_USRP , data , end_index - start_index , point_position ) ;

        }
        else if ( para_count == 2){
            str2double( fc , data , end_index - start_index , point_position ) ;
        }
        else if ( para_count == 3){
            str2float( RolloffFactor , data , end_index - start_index , point_position) ;         
        }
        else if ( para_count == 4){
            str2int( resample_point , data , end_index - start_index ) ;            
        }
        else if ( para_count == 5){
            str2int( detect_type  , data , end_index - start_index ) ;
        }
        else if ( para_count == 6){
            str2int( filelength  , data , end_index - start_index ) ;
        }
        else if ( para_count == 7){
            str2int( FilterSpanInSymbols , data , end_index - start_index) ;

        }
        else if ( para_count == 8){
            data = data.substr( 1 , data.size() - 2 ) ;
            const int length_MOD = data.length();
            MOD_type = new char[length_MOD+1];
            strcpy(MOD_type,data.c_str());
        }
        else if ( para_count == 9){
            data = data.substr( 1 , data.size() - 2 ) ;
            const int length_f = data.length();
            file_read = new char[length_f+1];
            strcpy(file_read,data.c_str());
        }
        else if ( para_count == 10){
            data = data.substr( 1 , data.size() - 2 ) ;
            const int length_s = data.length();
            file_save = new char[length_s+1];
            strcpy(file_save,data.c_str());
        }        
        else if ( para_count == 11){
            str2int( test_type_pilot  , data , end_index - start_index ) ;
            if ( test_type_pilot == 1 ){
                pilot_switch = 1 ;    
            }
            
        }
        else if ( para_count == 12 && pilot_switch == 1 ){
            str2int( space_type  , data , end_index - start_index ) ;
        }
        else if ( para_count == 13 && pilot_switch == 1 ){
            data = line.substr( 1 , end_index - start_index + 1 ) ;
            str2pilot( MOD_type , space_type , data , end_index - start_index , pilot_r , pilot_i  , inpilot_r , inpilot_i , pilot_string ) ;
        
            space_type = space_type - 2 ;

        }
            para_count = para_count + 1 ;


    
    }













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

    file.close();





    if ( test_type_pilot == 0 ){

        /*************************************Pilot Detection***************************************************/

        /********************          Read File          ********************/
        
        struct  timeval start_pilot_detection;
        struct  timeval end_pilot_detection;
        unsigned long diff_pilot_detection;
        gettimeofday(&start_pilot_detection,NULL); 


        int detect_filelength = 4000000;
        vector<int> rawDataI_pilot_detection(detect_filelength/2);//int *rawDataI = (int*)malloc(filelength/2 * sizeof(int)); //int rawDataI[filelength/2]; // cannot work
        vector<int> rawDataQ_pilot_detection(detect_filelength/2);//int *rawDataI = (int*)malloc(filelength/2 * sizeof(int)); //int rawDataI[filelength/2]; // cannot work
        DataConversion(filename, detect_filelength, rawDataI_pilot_detection, rawDataQ_pilot_detection); 
        /********************       Down Conversion        ********************/
        vector<double> DataDownconversion_pilot_detection_I(detect_filelength/2);//double *DataDownconversion_I = (double*)malloc(filelength/2 * sizeof(double));
        vector<double> DataDownconversion_pilot_detection_Q(detect_filelength/2);//double *DataDownconversion_Q = (double*)malloc(filelength/2 * sizeof(double));
        DownConversion(rawDataI_pilot_detection, rawDataQ_pilot_detection, DataDownconversion_pilot_detection_I, DataDownconversion_pilot_detection_Q, detect_filelength/2, fc, fs_USRP);

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
        resample( rsmooth_pilot_detection_I , rsmooth_pilot_detection_Q , resample_signal_pilot_detection_I , resample_signal_pilot_detection_Q , resample_factor , 3 );
            
        // free memorystrcmp
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


        
        //cout<< detect_type <<endl;





        vector<double> new_resample_I ;
        vector<double> new_resample_Q ; 
        vector<int> new_signal_raw_I ;
        vector<int> new_signal_raw_Q ; 
        

        int test_len = 1*pow(10,6) ;
        if (resample_signal_pilot_detection_I.size() > test_len ){
           
            for (int i = 0 ; i < test_len ; i++){
                new_resample_I.push_back(resample_signal_pilot_detection_I[i]) ;
                new_resample_Q.push_back(resample_signal_pilot_detection_Q[i]) ;
                new_signal_raw_I.push_back(rawDataI_pilot_detection[i]) ;
                new_signal_raw_Q.push_back(rawDataQ_pilot_detection[i]) ;
            }

        }else if (resample_signal_pilot_detection_I.size() < test_len ){    
            for (int i = 0 ; i < resample_signal_pilot_detection_I.size() ; i++){
                new_resample_I.push_back(resample_signal_pilot_detection_I[i]) ;
                new_resample_Q.push_back(resample_signal_pilot_detection_Q[i]) ;                
                new_signal_raw_I.push_back(rawDataI_pilot_detection[i]) ;
                new_signal_raw_Q.push_back(rawDataQ_pilot_detection[i]) ;
            }
        }



        double power = 0 ;
        if ( detect_type == 0 ){
            char temp_pilot = 'A' ;
            double temp_power = 0 ;
            pilot_detection_QPSK( new_resample_I , new_resample_Q , IPOINT , temp_pilot , temp_power );
            pilot_type = temp_pilot;

        }else if ( detect_type == 1 ){
            char temp_pilot = 'A' ;
            double temp_power = 0 ;
            pilot_detection_8PSK( new_resample_I , new_resample_Q , IPOINT , temp_pilot , temp_power );
            pilot_type = temp_pilot;

            
        }else if ( detect_type == 2 ){
            char temp_pilot = 'A' ;
            double temp_power = 0 ;
            double power = -1 ;
            for ( int i = 0 ; i < 5 ; i ++){
                temp_power = OQPSK_pilot_avg_power( i+1 , pilot_string, fc, fsy, fs_USRP, file_read, 1000000);
                if( temp_power > power ){
                    temp_pilot = 'Q' + i ;
                    power = temp_power ;
                }

            }


            pilot_type = temp_pilot;

            
        }else if ( detect_type == 3 ){
            char temp_pilot = 'A' ;
            double temp_power = 0 ;
            double power = -1 ;




            pilot_detection_8PSK( new_resample_I , new_resample_Q , IPOINT , temp_pilot , temp_power );

            if ( QPSKor8PSK(new_resample_I , new_resample_Q , temp_pilot ,  IPOINT  ,  MOD_type , fs_USRP ,  resample_factor) == 8 ){
                pilot_type = temp_pilot ;
                MOD_type = "8PSK" ;
                //cout << "8PSK" << endl ;
                power = temp_power ;
            }

            pilot_detection_QPSK( new_resample_I , new_resample_Q , IPOINT , pilot_type , power );
            if ( QPSKor8PSK(new_resample_I , new_resample_Q , pilot_type ,  IPOINT  ,  MOD_type , fs_USRP ,  resample_factor) == 4 ){
                pilot_type = temp_pilot ;
                MOD_type = "QPSK" ;
                cout << "QPSK" << endl ;
                power = temp_power ;
            }
            


            for ( int i = 0 ; i < 5 ; i ++){
                temp_power = OQPSK_pilot_avg_power( i+1 , pilot_string, fc, fsy, fs_USRP, file_read, 1000000);
                if( temp_power > power ){
                    temp_pilot = 'Q' + i ;
                    power = temp_power ;
                }
            }


        }




        // data_type = pilot_detection( resample_signal_pilot_detection_I, resample_signal_pilot_detection_Q, IPOINT , detect_type ,rawDataI_pilot_detection ,rawDataQ_pilot_detection);
        // printf("Pilot type is ");
        
        cout<< "Pilot type is : " << pilot_type << endl;

        pilot(pilot_type , pilot_r, pilot_i, pilot_string, inpilot_r, inpilot_i, space_type);
        





        gettimeofday(&end_pilot_detection,NULL);
        diff_pilot_detection = 1000000 * (end_pilot_detection.tv_sec-start_pilot_detection.tv_sec)+ end_pilot_detection.tv_usec-start_pilot_detection.tv_usec;     
        double diff_pilot_detection_result = diff_pilot_detection/1000000.0 ;
        //printf("PIlot type is found in %f seconds!!!!!!!!!!\n",diff_pilot_detection_result);    
        // free memory
        rawDataI_pilot_detection.clear();
        rawDataI_pilot_detection.shrink_to_fit();
        rawDataQ_pilot_detection.clear();
        rawDataQ_pilot_detection.shrink_to_fit();

    }
    

    // for( int i = 0 ; i < pilot_i.size() ; i++ ){
    //     cout<< pilot_i[i] ;
    // }
    // cout << endl ;
    // for( int i = 0 ; i < pilot_r.size() ; i++ ){
    //     cout<< pilot_r[i] ;
    // }
    // cout <<endl ;





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
        int burst_num = pilot_position.size() ;        
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
        

        int point_num_burst = inpilot_i.size();
        int num_burst = inpilot_i.size()/30 ;
        vector<vector<double>> processed_data_recieved_I( burst_num , vector<double> (point_num_burst));
        vector<vector<double>> processed_data_recieved_Q( burst_num , vector<double> (point_num_burst));
        vector<vector<int>> processed_symbol_recieved( burst_num , vector<int> (point_num_burst));
        //vector<double> processed_data_recieved_I(burst_num , point_num_burst) ;
        //vector<double> processed_data_recieved_Q(burst_num , point_num_burst) ;
        //vector<int> processed_symbol_recieved(burst_num , point_num_burst) ;
        cout<< burst_num <<endl;
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
    else if (  pilot_type > 'P' || data_para == 1 ){        
        // Let the number of demod symbol is same in each burst.
        /*
        vector<double> ReceivedSym_d;
        // OQPSK_main(fc, fsy, fs, filename, filelength);
        //ReceivedSym_d = OQPSK_main(fc, fsy, fs, filename, filelength); //(OK)
        ReceivedSym_d = OQPSK_main_2( pilot_type - 'P' , pilot_string , fc, fsy, fs_USRP, file_read, filelength);

        /////////////////////// write OQPSK demod result into file  //////////////////////////// 
        ofstream OQPSK_demod_result;
        OQPSK_demod_result.open ("OQPSK_demod_result.txt");
        OQPSK_demod_result << "OQPSK_demod_result\n";
        int count = 0;
        for(int i = 0; i < ReceivedSym_d.size() ;i++){
            OQPSK_demod_result << ReceivedSym_d[i] << " ";
            count = count + 1;
            if(count == 50){
               OQPSK_demod_result << "\n"; 
               count = 0;
            }
        }
        OQPSK_demod_result.close();
        */
        // Find the start and end position of the burst and demd them, so the number of demod symbol is different in each burst.
        vector<double> ReceivedSym_d;
        vector<int> demd_symbol_per_burst;
        vector<int> coarse_pilot_position;
        vector<int> end_burst_position;
        // ReceivedSym_d = OQPSK_main_3(pilot_type - 'P' , pilot_string, fc, fsy, fs_USRP, file_read, filelength, demd_symbol_per_burst, coarse_pilot_position, end_burst_position); 
        // ReceivedSym_d = OQPSK_main_4(pilot_type - 'P', pilot_string, fc, fsy, fs_USRP, file_read, filelength, demd_symbol_per_burst, coarse_pilot_position, end_burst_position);
        ReceivedSym_d = OQPSK_main_5(pilot_type - 'P', pilot_string, fc, fsy, fs_USRP, file_read, filelength, demd_symbol_per_burst, coarse_pilot_position, end_burst_position);
    
        /////////////////////// write OQPSK demod result into file  //////////////////////////// 
        ofstream OQPSK_demod_result;
        //OQPSK_demod_result.open ("OQPSK_demod_result.txt");
        OQPSK_demod_result.open (filename_save);
        OQPSK_demod_result << "OQPSK_demod_result" << endl;
        int count;
        int nextline_count;
        for(int j = 0; j < coarse_pilot_position.size(); j++){
            OQPSK_demod_result << "fc:" << fc << " burst" << j+1 << "\n";
            OQPSK_demod_result << "burst interval: " << coarse_pilot_position[j] << " ~ " << end_burst_position[j] << "\n";
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

        ReceivedSym_d.clear(); vector<double>(ReceivedSym_d).swap(ReceivedSym_d);
    }
    return 0;

}
