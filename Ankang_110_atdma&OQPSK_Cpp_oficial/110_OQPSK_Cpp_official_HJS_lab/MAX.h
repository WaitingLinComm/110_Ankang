/* 
 * max
 *  x: input vector
 *  max: maximum value of elements among the input vector
 *  max_index: the index of the maximum value
 *  L: length(x)
 */
#pragma once
#include <stdio.h>
#include <vector>
#include<bits/stdc++.h>
using namespace std;
void MAX(vector<double> &x, double &max_value, int &max_index)
{
    int L = x.size();
    double tmp_max = x[0];
    int tmp_max_index = 0;
    for(int i = 0; i < L; i++)
    {
        if(x[i] > tmp_max)
        {
            tmp_max = x[i];
            tmp_max_index = i;
        }
    }
    max_value = tmp_max;
    max_index = tmp_max_index;
}

