/* 
 * convolution: realize z = conv(x, y) in matlab
 * 
 *  Input variable:
 *  x: input signal
 *  y: filter
 *  z: filter result
 * 
 */
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include<bits/stdc++.h>
#include <omp.h>
#include <iostream>
using namespace std;
void pilot_convolution(vector<double> &input_sig, vector<double> &filter, vector<double> &output_sig , int space_type , int IPOINT )
{
	int input_sig_len = input_sig.size();
	int filter_len = filter.size();
	int output_sig_len = output_sig.size();
    int filter_burst = filter_len/30/IPOINT ;
	// vector<double> temp_sig( input_sig_len + filter_len -1 ) ;
 //    #pragma omp parallel for
	// for (int i = 0 ; i < input_sig_len ; i++ ){
	// 	temp_sig[i] = input_sig[i]; 
	// }


    //temp_sig( input_sig_len + filter_len -1 )
    
    if ( space_type != 0){
        #pragma omp parallel for
        for ( int ind_conv_result = 0 ; ind_conv_result < input_sig_len - filter_len ; ind_conv_result++ ){
            double temp = 0 ;
            //#pragma omp parallel for
            for ( int burst_index = 0 ; burst_index < filter_burst ; burst_index++ ){
                //#pragma omp parallel for
                for ( int ind_in_burst = 0 ; ind_in_burst < space_type ; ind_in_burst++){   
                    temp = temp + filter[ IPOINT*( 30*burst_index + ind_in_burst ) ] * input_sig[ ind_conv_result + IPOINT*( 30*burst_index + ind_in_burst )];
                    //output_sig[ind_conv_result] = output_sig[ind_conv_result] + filter[ IPOINT*( 30*burst_index + ind_in_burst ) ] * input_sig[ ind_conv_result + IPOINT*( 30*burst_index + ind_in_burst )];
                }
            }
            //printf("%")
            output_sig[ind_conv_result] = temp ;

        }


    } 
    else{
    	#pragma omp parallel for
        for ( int ind_conv_result = 0 ; ind_conv_result < input_sig_len -filter_len ; ind_conv_result++ ){	
            for ( int burst_index = 0 ; burst_index < filter_len/IPOINT ; burst_index++ ){
                output_sig[ind_conv_result]  = output_sig[ind_conv_result]  + filter[ IPOINT*burst_index] * input_sig[ ind_conv_result + IPOINT*burst_index];
            }




    }




}

}
