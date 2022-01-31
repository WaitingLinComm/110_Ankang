#pragma once
#include <stdio.h>
#include <vector>
//#include<bits/stdc++.h>
using namespace std;
void MIN(vector<double> &x, double &min_value, int &min_index)
{
    int L = x.size();
    double tmp_min = x[0];
    int tmp_min_index = 0;
    for(int i = 0; i < L; i++)
    {
        if(x[i] < tmp_min)
        {
            tmp_min = x[i];
            tmp_min_index = i;
        }
    }
    min_value = tmp_min;
    min_index = tmp_min_index;
}
