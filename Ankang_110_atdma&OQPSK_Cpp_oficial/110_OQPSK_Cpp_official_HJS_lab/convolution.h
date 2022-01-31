/* 
 * convolution: realize z = conv(x, y) in matlab
 * 
 *  Input variable:
 *  x: input signal
 *  y: filter
 *  z: filter result
 * 
 *  Lx: length of input signal
 *  Ly: length of filter
 *  Lz: length of filter result
 * 
 *  NOTE: Lz = Lx + Ly - 1
 * 
 */
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include<bits/stdc++.h>
#include <omp.h>
using namespace std;
void convolution(vector<double> &x, vector<double> &y, vector<double> &z)
{
	int x_len = x.size();
	int y_len = y.size();
	int z_len = z.size();

    #pragma omp parallel for
	for ( int ind_conv_result = 0 ; ind_conv_result < x_len - y_len; ind_conv_result++ ){
		//#pragma omp parallel for
		for ( int sum_index = 0 ; sum_index < y_len ; sum_index++ ){
			z[ind_conv_result] = z[ind_conv_result] + y[sum_index] * x[ sum_index + ind_conv_result];
		}
	}
	
	
	



}
