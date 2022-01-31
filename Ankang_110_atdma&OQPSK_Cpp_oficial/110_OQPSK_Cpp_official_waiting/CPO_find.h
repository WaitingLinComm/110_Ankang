#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
//#include<bits/stdc++.h>
using namespace std;
double CPO_find( vector<double> input_signal_I,vector<double> input_signal_Q)
{
    double imag_part=0 ;
    double real_part=0 ;
    int data_len = input_signal_I.size();
    for( int data_index = 0 ; data_index < data_len ; data_index++ ){
        real_part = real_part + input_signal_I[data_index] ;
        imag_part = imag_part + input_signal_Q[data_index] ;
    } 
    double phase = atan2( imag_part , real_part ) ;
    return phase ;
}
