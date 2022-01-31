/* 
 * DownConversion
 * 
 * Input variable:
 * int *InData_I: real part of InData before down convertor
 * int *InData_Q: imaginary part of InData before down convertor
 * 
 * double *OutData_I: real part of OutData after down convertor
 * double *OutData_Q: imaginary part of OutData after down convertor
 * 
 * int DataLength: length of InData_I and InData_Q
 * 
 * fc: center frequency of subcarrier
 * fs: sampling frequency
 */
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <vector>
#include<bits/stdc++.h>
using namespace std;
#define pi 3.14159265358979323846

void DownConversion(vector<int> &InData_I, vector<int> &InData_Q, vector<double> &OutData_I, vector<double> &OutData_Q, int DataLength, double fc, double fs)
{
    double ff = 2 * pi * fc / fs;

    #pragma omp parallel for
    for (int i = 0; i < DataLength; i++)
    {    
	double coscoff, sincoff;
    	coscoff = cos((i + 1) * ff); // NOTE: in matlab: Sig_demod = InSig .* exp(-1i * 2 * pi * Ts * f_c * (1 : length(InSig)));
    	sincoff = sin((i + 1) * ff);

        OutData_I[i] = InData_I[i] * coscoff + InData_Q[i] * sincoff;
        OutData_Q[i] = - InData_I[i] * sincoff + InData_Q[i] * coscoff;
    }
}
