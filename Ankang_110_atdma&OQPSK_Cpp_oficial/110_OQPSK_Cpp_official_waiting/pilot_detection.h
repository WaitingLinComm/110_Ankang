#pragma once
#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include "convolution.h"
#include "Upsample.h"
#include "pilot.h"
#include "pilot_convolution.h"
#include "find_high_pilot.h"
#include "abs_mean.h"
#include "point_divide_real.h"
#include "point_divide_imag.h"
#include "point_multiple_real.h"
#include "point_multiple_imag.h"
#include "pilot_compare.h"
#include "QPSKor8PSK.h"
#include "pilot_detection_QPSK.h"
#include "pilot_detection_8PSK.h"
//#include "pilot_detection_OQPSK.h"
//#include<bits/stdc++.h>

using namespace std;    
    
char pilot_detection(vector<double> &resample_signal_I, vector<double> &resample_signal_Q, int IPOINT , int detect_type){
// return pilot type ; detailed type is in "pilot.h" 
// detect_type = 0 for atdma_QPSK 
// detect_type = 1 for atdma_8PSK 
// detect_type = 2 for OQPSK 
// detect_type = 3 for blind detection   

// test 3*10^5 data
    vector<double> new_resample_I ;
    vector<double> new_resample_Q ; 
int test_len = 1*pow(10,6) ;
if (resample_signal_I.size() > test_len ){
   
    for (int i = 0 ; i < test_len ; i++){
        new_resample_I.push_back(resample_signal_I[i]) ;
        new_resample_Q.push_back(resample_signal_Q[i]) ;
    }

}else if (resample_signal_I.size() < test_len ){    
    for (int i = 0 ; i < resample_signal_I.size() ; i++){
        new_resample_I.push_back(resample_signal_I[i]) ;
        new_resample_Q.push_back(resample_signal_Q[i]) ;
    }
}



char pilot_type = 'A' ;
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
    //pilot_detection_OQPSK( new_resample_I , new_resample_Q , IPOINT , temp_pilot , temp_power );
    pilot_type = temp_pilot;

    
}else if ( detect_type == 3 ){
    char temp_pilot = 'A' ;
    double temp_power = 0 ;
    pilot_detection_QPSK( new_resample_I , new_resample_Q , IPOINT , pilot_type , power );

    pilot_detection_8PSK( new_resample_I , new_resample_Q , IPOINT , temp_pilot , temp_power );
    if ( temp_power > power && QPSKor8PSK(new_resample_I , new_resample_Q ) == 8 ){
        pilot_type = temp_pilot ;
    }

    //pilot_detection_OQPSK( new_signal_raw_I, new_signal_raw_Q , IPOINT , temp_pilot , temp_power );
    if (temp_power > power ){
        pilot_type = temp_pilot ;
    }
}

return pilot_type;




}
