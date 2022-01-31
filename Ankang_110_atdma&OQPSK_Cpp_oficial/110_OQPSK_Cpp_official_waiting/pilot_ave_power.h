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
//#include<bits/stdc++.h>
using namespace std;    
    
double pilot_ave_power(char type, vector<double> &resample_signal_I, vector<double> &resample_signal_Q, int IPOINT){
// return 0 for no detection
// return a double for average power of pilot dectection
    vector<double> pilot_r;
    vector<double> pilot_i;
    vector<int> pilot_string;
    vector<double> inpilot_r;
    vector<double> inpilot_i;
	int space_type = 0 ;
    pilot(type , pilot_r, pilot_i, pilot_string, inpilot_r, inpilot_i, space_type);
   

    
    vector<double> inpilot_r_UP(inpilot_r.size()*IPOINT);
    vector<double> inpilot_i_UP(inpilot_i.size()*IPOINT);
    Upsample(inpilot_r, inpilot_r_UP, IPOINT);
    Upsample(inpilot_i, inpilot_i_UP, IPOINT);
    
	vector<double> MF(resample_signal_I.size()) ;
    vector<double> MF_real(resample_signal_I.size()) ;
    vector<double> MF_imag(resample_signal_I.size()) ;
    vector<double> MF_conv_1(resample_signal_I.size()) ;
    vector<double> MF_conv_2(resample_signal_I.size()) ;
    vector<double> MF_conv_3(resample_signal_I.size()) ;
    vector<double> MF_conv_4(resample_signal_I.size()) ;
	
    pilot_convolution(resample_signal_I , inpilot_r_UP , MF_conv_1, space_type, IPOINT) ;
    pilot_convolution(resample_signal_Q , inpilot_i_UP , MF_conv_2, space_type, IPOINT) ;
    pilot_convolution(resample_signal_I , inpilot_i_UP , MF_conv_3, space_type, IPOINT) ;
    pilot_convolution(resample_signal_Q , inpilot_r_UP , MF_conv_4, space_type, IPOINT) ;

    
    

    for (int i = 0 ; i < MF_real.size() ; i++ ){ // conv( s_r + 1j* s_i , conj( p_r , 1j* p_i )   ) = conv(s_r,p_r)+conv(s_i,p_i) -1j*conv(s_r,p_i)+1j*conv(s_i,p_r)
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

    int pilot_num = pilot_position.size();
    double pilot_detect_power = 0 ;
    //cout << pilot_num << endl ;
    if (pilot_num == 0 || pilot_num == 1){
        pilot_detect_power = 0 ;
    }else{
        for(int pilot_index = 0 ; pilot_index < pilot_num ; pilot_index++){
            pilot_detect_power = pilot_detect_power + MF[ pilot_position[pilot_index] ]/log(pilot_num)/pilot_r.size();
            //cout << " "<< MF[ pilot_position[pilot_index] ]/ log(pilot_num)/pilot_r.size() << " ";
        }

    }
    //cout<< pilot_num<<endl ;
    //cout << endl << "pilot power is " << pilot_detect_power << endl ;
    return pilot_detect_power ;


    
}    
    
    
