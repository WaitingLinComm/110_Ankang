#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <omp.h>
#include <vector>
//#include<bits/stdc++.h>
using namespace std;
double abs_mean(vector<double> &x,vector<double> &y)
{
    double sum_temp = 0;
    #pragma omp parallel for
    for (int i = 0 ; i < x.size() ; i++)
    {
        sum_temp = sum_temp + sqrt( pow(x[i],2) + pow(y[i],2) ) ;
    }
    sum_temp = sum_temp / x.size() ;
    return sum_temp;
}
