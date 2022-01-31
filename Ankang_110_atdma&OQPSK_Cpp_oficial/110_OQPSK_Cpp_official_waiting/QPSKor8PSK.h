#pragma once
#include <stdio.h>
#include <iostream>
#include<sys/time.h>
#include<unistd.h>
#include <vector>
#include <stdlib.h>
//#include<bits/stdc++.h>
#include <math.h>
//#include <omp.h>
#include "point_multiple.h"
#include "variance_for_constellation.h"
#include "Upsample.h"
#include "pilot_convolution.h"
#include "pilot.h"
#include "Demodulate.h"
using namespace std;
int QPSKor8PSK(vector<double> &resample_signal_I , vector<double> &resample_signal_Q ,  char pilot_type , int IPOINT  , char MOD_type[] , double fs_USRP , double resample_factor){
	
	double parameter1 = 0 ;
	double parameter2 = 0 ;
	double temp_real = 0 ;
	double temp_imag = 0 ;
	double real_45 = cos(3.14159/4) ;
	double imag_45 = sin(3.14159/4) ;
	int space_type ;

    vector<double> pilot_r;
    vector<double> pilot_i;
    vector<int> pilot_string;
    vector<double> inpilot_r;
    vector<double> inpilot_i;

    //cout << pilot_type <<endl ;

	pilot(pilot_type , pilot_r, pilot_i, pilot_string, inpilot_r, inpilot_i, space_type);


	// for ( int i = 0 ; i<pilot_string.size() ; i++){
	// 	cout<<pilot_string[i] ;
	// }
	// cout<< endl<< space_type << endl ;



        vector<double> inpilot_r_UP( inpilot_i.size()*IPOINT);
        vector<double> inpilot_i_UP( inpilot_i.size()*IPOINT);
        Upsample( inpilot_r , inpilot_r_UP, IPOINT);
        Upsample( inpilot_i , inpilot_i_UP, IPOINT);


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

  

        MF.clear();
        MF.shrink_to_fit();
        
        /********************     Demodulation    ********************/ 

        int burst_num = pilot_position.size() ;
        int point_num_burst = inpilot_i.size();
        int num_burst = inpilot_i.size()/30 ;
        vector<vector<double>> processed_data_recieved_I( burst_num , vector<double> (point_num_burst));
        vector<vector<double>> processed_data_recieved_Q( burst_num , vector<double> (point_num_burst));
        vector<vector<int>> processed_symbol_recieved( burst_num , vector<int> (point_num_burst));
        //vector<double> processed_data_recieved_I(burst_num , point_num_burst) ;
        //vector<double> processed_data_recieved_Q(burst_num , point_num_burst) ;
        //vector<int> processed_symbol_recieved(burst_num , point_num_burst) ;

        for( int demod_index = 0 ; demod_index < burst_num ; demod_index++ ){
        	//cout<< endl << "burst index is " << demod_index << "\n" ;
            Demodulate( pilot_position , resample_signal_I , resample_signal_Q , point_num_burst , num_burst , IPOINT ,  fs_USRP*resample_factor, pilot_r, pilot_i, inpilot_r, inpilot_i ,MOD_type , processed_data_recieved_I , processed_data_recieved_Q , processed_symbol_recieved , demod_index);
    	}


        vector<double> vec_b_i_hat_p_K( burst_num * point_num_burst ) ;
        vector<double> vec_b_q_hat_p_K( burst_num * point_num_burst ) ;


        for( int i = 0 ; i < burst_num ; i++ ){
            for( int j = 0 ; j < point_num_burst ; j++ ){
                vec_b_i_hat_p_K[ i * point_num_burst + j ] =  processed_data_recieved_I[i][j];
                vec_b_q_hat_p_K[ i * point_num_burst + j ] =  processed_data_recieved_Q[i][j];
            }
        }



	// vector<double> parameter1_I(vec_b_i_hat_p_K.size());
	// vector<double> parameter1_Q(vec_b_i_hat_p_K.size());
	// vector<double> parameter2_I(vec_b_i_hat_p_K.size());
	// vector<double> parameter2_Q(vec_b_i_hat_p_K.size());
	int para = 0 ;
	for( int data_index = 0 ; data_index < vec_b_i_hat_p_K.size() ; data_index++ ){
	

		if ( abs( atan2( vec_b_i_hat_p_K[data_index] , vec_b_q_hat_p_K[data_index] ) ) < 3.14159/8  ){

			para = para + 1 ;
		}
		// point_multiple( temp_real , temp_imag , vec_b_i_hat_p_K[data_index] , vec_b_q_hat_p_K[data_index] , real_45 , imag_45) ;
		// point_multiple( temp_real , temp_imag , temp_real , temp_imag ,temp_real , temp_imag ) ;
		// point_multiple( temp_real , temp_imag , temp_real , temp_imag ,temp_real , temp_imag ) ;
		// point_multiple( temp_real , temp_imag , temp_real , temp_imag ,temp_real , temp_imag ) ;

		// parameter1_I[data_index] =  temp_real  ;
		// parameter1_Q[data_index] =  temp_imag  ;

	
		// point_multiple( temp_real , temp_imag ,vec_b_i_hat_p_K[data_index] , vec_b_q_hat_p_K[data_index] , vec_b_i_hat_p_K[data_index] , vec_b_q_hat_p_K[data_index]  ) ;
		// point_multiple( temp_real , temp_imag , temp_real , temp_imag ,temp_real , temp_imag ) ;
		// point_multiple( temp_real , temp_imag , temp_real , temp_imag ,temp_real , temp_imag ) ;


		// parameter2_I[data_index] =  temp_real  ;
		// parameter2_Q[data_index] =  temp_imag  ;


		       

	}
	double para2 = (double)para/vec_b_i_hat_p_K.size() ;
	// parameter1 = variance_for_constellation( parameter1_I, parameter1_Q) ;
	// parameter2 = variance_for_constellation( parameter2_I, parameter2_Q) ;
 // 	cout << "para1:" << parameter1 << endl ;
 // 	cout << "para2:" << parameter2 << endl ;
	// cout << "para:" << para2 << endl ;
	if (  para2 < 1/10 ){
		return 4 ;
	}
	else {
		return 8 ;
	}



} 