#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include<bits/stdc++.h>
#include "point_multiple_real.h"
#include "point_multiple_imag.h"
#define pi 3.14159265358979323846
using namespace std;
void phase_shift(vector<double> input_signal_I,vector<double> input_signal_Q, vector<double> &output_signal_I,vector<double> &output_signal_Q, double phase)
{
    int signal_len = input_signal_I.size();
    double phase_I = cos( phase );
    double phase_Q = sin( phase );
    
    for( int data_index = 0 ; data_index < signal_len ; data_index++ ){
        output_signal_I[data_index] = point_multiple_real( input_signal_I[data_index] , input_signal_Q[data_index] , phase_I , phase_Q );
        output_signal_Q[data_index] = point_multiple_imag( input_signal_I[data_index] , input_signal_Q[data_index] , phase_I , phase_Q );
    } 
}
