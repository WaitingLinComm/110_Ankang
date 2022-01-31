#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include<bits/stdc++.h>
#define pi 3.14159265358979323846
using namespace std;
void pilot_compare( vector<double> input1_signal_I,vector<double> input1_signal_Q, vector<double> input2_signal_I,vector<double> input2_signal_Q, vector<double> &output_signal_I,vector<double> &output_signal_Q )
{
    int signal_len = input1_signal_I.size();
    for( int data_index = 0 ; data_index < signal_len ; data_index++ ){
        output_signal_I[data_index] = point_divide_real( input1_signal_I[data_index] , input1_signal_Q[data_index] , input2_signal_I[data_index] , input2_signal_Q[data_index] );
        output_signal_Q[data_index] = point_divide_imag( input1_signal_I[data_index] , input1_signal_Q[data_index] , input2_signal_I[data_index] , input2_signal_Q[data_index] );
    } 
}
