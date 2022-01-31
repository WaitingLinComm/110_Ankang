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
#include "pilot_ave_power.h"
#include <iostream>
//#include<bits/stdc++.h>
using namespace std;  


void pilot_detection_QPSK( vector<double> resample_signal_I , vector<double>  resample_signal_Q , int IPOINT , char &pilot , double &power ){

double power_temp = 0 ;
for ( int char_ind = 65 ; char_ind < 65+8 ; char_ind++ ){

    power_temp = pilot_ave_power( char_ind, resample_signal_I, resample_signal_Q, IPOINT );
    //cout << power_temp<< " "<< power << " "<< (char)char_ind << endl ;
    if (power_temp > power){ 
        pilot = char_ind ;
        power = power_temp ;
    }
}

}
