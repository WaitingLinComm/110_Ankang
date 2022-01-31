#pragma once
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
#include "Downsample.h"
#include "Upsample.h"
#include "resample.h"
#include "variance_for_constellation.h"
#include "pilot.h"
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
#include "Print_vector.h"
#include<bits/stdc++.h>
using namespace std;


void Demodulate( vector<int> &pilot_position , vector<double> &resample_signal_I , vector<double> &resample_signal_Q , int point_num_burst , int num_burst , int IPOINT , double nfs_USRP, vector<double> &pilot_r, vector<double> &pilot_i, vector<double> &inpilot_r, vector<double> &inpilot_i , char MOD_type[] ,  vector<vector<double>> &processed_data_recieved_I , vector<vector<double>> &processed_data_recieved_Q , vector<vector<int>> &processed_symbol_recieved , int demod_index ){
    // locate pilot position
    int init_pos_burst = pilot_position[ demod_index ] ;
    // find data symbol       
    vector<double> demod_data_I;
    vector<double> demod_data_Q;        
    Downsample( resample_signal_I , demod_data_I , init_pos_burst , point_num_burst , IPOINT );
    Downsample( resample_signal_Q , demod_data_Q , init_pos_burst , point_num_burst , IPOINT );
    // cout << demod_data_I.size(); 
    
    
    // test if current position is pilot position (MF peak value)  
    //int a = 0 ; int b = 0 ;
    //for (int i = 0 ; i < demod_data_I.size() ; i++ 	){
    //	a += point_multiple_real( demod_data_I[i] , demod_data_Q[i] , inpilot_r[i] , -inpilot_i[i]  ) ;
    //	b += point_multiple_imag( demod_data_I[i] , demod_data_Q[i] , inpilot_r[i] , -inpilot_i[i]  ) ;    	    	
	//}
	//cout << sqrt( pow( a , 2 ) + pow( b , 2 ) ) <<endl;


    // use pilot to detect 
    
    
    vector<double> time_data(point_num_burst); // init time corresponding to resampled data

    for( int num = 0 ; num < point_num_burst ; num++ ){
        time_data[ num ] = ( init_pos_burst + IPOINT*num ) / nfs_USRP ;
    }
    

    
    vector<double> ind(num_burst); // locate first symbol of each distributed pilot       
    for( int num = 0 ; num < num_burst ; num++ ){
        ind[ num ] = 30*num ;
    }
	//printf("Time mapping is Done. \n");
    // extract pilots from received data / time index
    int symbol_per_burst = 2; 
    vector<double> datap_I(num_burst*symbol_per_burst);// pilot data 
    vector<double> datap_Q(num_burst*symbol_per_burst);        
    vector<double> tp(num_burst*symbol_per_burst);// pilot time value
    vector<double> cfodata_I(num_burst*symbol_per_burst);// divide pilot data by pilot
    vector<double> cfodata_Q(num_burst*symbol_per_burst);
    
	for( int num = 0 ; num < num_burst ; num++ ){
        for( int sym_ind = 0 ; sym_ind < symbol_per_burst ; sym_ind++ ){
            datap_I[ symbol_per_burst*num + sym_ind ]=demod_data_I[30*num+sym_ind];
            datap_Q[ symbol_per_burst*num + sym_ind ]=demod_data_Q[30*num+sym_ind];                
            tp[ symbol_per_burst*num + sym_ind ]=time_data[30*num+sym_ind];
        }
    }
    
    for( int num = 0 ; num < num_burst ; num++ ){
        for( int sym_ind = 0 ; sym_ind < symbol_per_burst ; sym_ind++ ){
			cfodata_I[ symbol_per_burst*num + sym_ind ] = point_divide_real( datap_I[ symbol_per_burst*num + sym_ind ] ,datap_Q[ symbol_per_burst*num + sym_ind ], pilot_r[ symbol_per_burst*num + sym_ind ],pilot_i[ symbol_per_burst*num + sym_ind ]);
		    cfodata_Q[ symbol_per_burst*num + sym_ind ] = point_divide_imag( datap_I[ symbol_per_burst*num + sym_ind ] ,datap_Q[ symbol_per_burst*num + sym_ind ], pilot_r[ symbol_per_burst*num + sym_ind ],pilot_i[ symbol_per_burst*num + sym_ind ]);  		
        }
    }	
    //cout << pilot_r.size() << datap_Q.size() <<endl;
	//cout << sum(cfodata_I , 0 , cfodata_I.size() )  << endl ;	
	

	//printf("Pilot extraction is Done. \n");
    datap_I.clear();
    datap_I.shrink_to_fit();
    datap_Q.clear();
    datap_Q.shrink_to_fit();

    
    // CFO value calculate
    double freq_bound = 1e4 ;
    int freq_spacing = 50 ;
    double var_temp = 0 ;
    vector<double> freq_offset( 2*freq_bound/freq_spacing + 1 );// CFO frequency range
    vector<double> cfodata_shift_temp_I( num_burst*symbol_per_burst );// pilot data after frequency shift      
    vector<double> cfodata_shift_temp_Q( num_burst*symbol_per_burst );                
    vector<double> objective_var( 2*freq_bound/freq_spacing + 1 );// variance of pilot data after frequency shift

    for( int freq_offset_test_index = 0 ; freq_offset_test_index < 2*freq_bound/freq_spacing + 1 ; freq_offset_test_index++ ){
        freq_offset[freq_offset_test_index] = freq_offset_test_index * freq_spacing - freq_bound ;
    }

    for( int freq_offset_test_index = 0 ; freq_offset_test_index < 2*freq_bound/freq_spacing + 1 ; freq_offset_test_index++ ){
        frequency_shift( cfodata_I , cfodata_Q , cfodata_shift_temp_I , cfodata_shift_temp_Q , freq_offset[freq_offset_test_index] , tp);
        objective_var[freq_offset_test_index] = variance_for_constellation( cfodata_shift_temp_I , cfodata_shift_temp_Q );
    }
	
	//Print_vector(objective_var) ;
	
    int min_index ;
    double min_value ;
    MIN( objective_var , min_value, min_index);
    double freq_offset_value = freq_offset[min_index];
	
	
    objective_var.clear();
    objective_var.shrink_to_fit();
    
	//printf("CFO is found. \n");
	
	
    // CPO value calculate
    frequency_shift( cfodata_I , cfodata_Q , cfodata_shift_temp_I , cfodata_shift_temp_Q , freq_offset_value , tp) ;
    double CPO_value = CPO_find( cfodata_shift_temp_I , cfodata_shift_temp_Q ) ; 

    tp.clear();
    tp.shrink_to_fit();
    freq_offset.clear();
    freq_offset.shrink_to_fit();        
    cfodata_shift_temp_I.clear();
    cfodata_shift_temp_I.shrink_to_fit();  
    cfodata_shift_temp_Q.clear();
    cfodata_shift_temp_Q.shrink_to_fit(); 
	
	
	//printf("CPO is found. \n");
    // CFO
    // Print_vector(time_data); 
    vector<double> data_after_cfo_I(point_num_burst) ;
    vector<double> data_after_cfo_Q(point_num_burst) ;
    frequency_shift( demod_data_I , demod_data_Q , data_after_cfo_I , data_after_cfo_Q , freq_offset_value , time_data);
	
	//cout << data_after_cfo_I[0] <<endl ;  	
	
	//printf("CFO is done. \n");
	//Print_vector(demod_data_I); 
	
	
    demod_data_I.clear();
    demod_data_I.shrink_to_fit();
    demod_data_Q.clear();
    demod_data_Q.shrink_to_fit();
		
	cfodata_I.clear();
    cfodata_I.shrink_to_fit();
    cfodata_Q.clear();
    cfodata_Q.shrink_to_fit();

    // CPO
    vector<double> data_after_cpo_I(point_num_burst) ;
    vector<double> data_after_cpo_Q(point_num_burst) ;
    phase_shift(  data_after_cfo_I , data_after_cfo_Q , data_after_cpo_I , data_after_cpo_Q , -CPO_value );

	//printf("CPO is done. \n");	
	

    // save data
    for( int data_index = 0 ; data_index < point_num_burst ; data_index++ ){
        processed_data_recieved_I[ demod_index ][ data_index ] = data_after_cpo_I[ data_index] ; 
        processed_data_recieved_Q[ demod_index ][ data_index ] = data_after_cpo_Q[ data_index] ; 
        processed_symbol_recieved[ demod_index ][ data_index ] = convert_to_index( data_after_cpo_I[ data_index] , data_after_cpo_Q[ data_index] , MOD_type) ;
        //cout << processed_symbol_recieved[ demod_index ][ data_index ] ;
    }
    
   
    
	//cout<< endl;
    time_data.clear();
    time_data.shrink_to_fit();
    data_after_cfo_I.clear();
    data_after_cfo_I.shrink_to_fit();
    data_after_cfo_Q.clear();
    data_after_cfo_Q.shrink_to_fit();        
    data_after_cpo_I.clear();
    data_after_cpo_I.shrink_to_fit();
    data_after_cpo_Q.clear();
    data_after_cpo_Q.shrink_to_fit();
 	
	//printf("Data is saved. \n");       
	//cout << endl << endl << endl ;
    }
