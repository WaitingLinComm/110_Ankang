#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <vector>
#include<bits/stdc++.h>
using namespace std;

double sum(vector<double> &x, int start_point , int end_point)
{
    double sum_tmp = 0;
    #pragma omp parallel for
    for (int i = start_point; i < end_point; i++)
    {
       sum_tmp = sum_tmp + x[i];
    }
    return sum_tmp;
}

