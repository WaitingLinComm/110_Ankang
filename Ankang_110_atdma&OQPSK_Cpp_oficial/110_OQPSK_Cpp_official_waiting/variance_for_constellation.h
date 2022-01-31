#pragma once
#include <stdio.h>
#include <vector>
#include "sum.h"
//#include<bits/stdc++.h>
using namespace std;
#define square(a) a*a

double variance_for_constellation( vector<double> &data_I, vector<double> &data_Q)
{
    int data_size = data_I.size();
    double var_value = 0 ;
    // center point
    double center_I = 0;
	center_I = sum(data_I , 0 , data_size)/data_size;
    double center_Q = 0 ;
	center_Q = sum(data_Q , 0 , data_size)/data_size;
	//cout << center_I << endl ; 
    // calculate var
    for ( int var_count = 0 ; var_count < data_size ; var_count++ ){
        var_value += (data_I[var_count]-center_I)*(data_I[var_count]-center_I);
        var_value += (data_Q[var_count]-center_Q)*(data_Q[var_count]-center_Q);
    }
    var_value = var_value / data_size;

	return var_value;
}
