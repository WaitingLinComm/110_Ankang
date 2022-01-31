/* 
 * SRRC
 * 
 * Input variable:
 * 
 * span: FilterSpanInSymbols
 * beta: RolloffFactor;
 * sps: SamplerPerSymbol
 * FilterLength: length of the pulse shaping filter
 * 
 */
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <vector>
#include<bits/stdc++.h>
using namespace std;
#define pi 3.14159265358979323846

void SRRC( vector<double> &PulseshapingFilter, int span, float beta, int sps, int FilterLength)
{
    int filterOrder = sps * span;
    double delay = filterOrder/2;
    vector<double> t(FilterLength);
    vector<double> p(FilterLength);
    double FilterPower = 0;
   
    #pragma omp parallel for
    for(int i = 0; i < FilterLength; i++)
    {   
        t[i] = (-delay + i)/sps; // time index of the filter 
        if(i == int(delay))
        {
            // SRRC Filter
            // The value of mid-point(n = 0). In matlab: p(delay + 1); In C: p[delay]. NOTE: the index of C is start from 0!
            p[i] = -1 / (pi * sps) * (pi * (beta - 1) - 4 * beta);
        }
        else        
        {
            // SRRC Filter
            p[i] = -4 * beta/sps * ( cos((1 + beta) * pi * t[i]) + sin((1-beta) * pi * t[i])/(4 * beta * t[i]))/(pi * (pow(4 * beta * t[i], 2) - 1)); 
        }
        FilterPower = FilterPower + pow(p[i], 2); 
    }
    double FilterEnergy = sqrt(FilterPower);
    
    double max_p_SRRC = p[int(delay)] / FilterEnergy; //max_p_SRRC equals to the mid-point(n = 0) value of p

    #pragma omp parallel for
    for(int i = 0; i < FilterLength; i++)
    {
        PulseshapingFilter[i] = p[i] / FilterEnergy ;  // Normalize filter energy and impulse response
    }
    t.clear();
	t.shrink_to_fit();
    p.clear();
	p.shrink_to_fit();



}
