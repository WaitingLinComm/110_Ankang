#pragma once
#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <math.h>
//#include<bits/stdc++.h>
using namespace std;    

void pilot_detection_OQPSK( vector<int> new_signal_raw_I , vector<int>  new_signal_raw_Q , int IPOINT , char &pilot , double &power ){


vector<double> new_signal_raw_I_double(new_signal_raw_I.size());
vector<double> new_signal_raw_Q_double(new_signal_raw_Q.size());

for(int i =1; i< new_signal_raw_I.size();i++){
    new_signal_raw_I_double[i] = (double)new_signal_raw_I[i];
    new_signal_raw_Q_double[i] = (double)new_signal_raw_Q[i];
}    

OQPSK_pilot_ave_power( new_signal_raw_I_double, new_signal_raw_Q_double, IPOINT );

/*
int power_temp = 0 ;
for ( int char_ind = 81 ; char_ind < 81+5 ; char_ind++ ){
    power_temp = pilot_ave_power( char_ind, resample_signal_I, resample_signal_Q, IPOINT );
    if (power_temp > power){ 
        pilot = char_ind ;
        power = power_temp ;
    }
}
*/
}
