#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include<bits/stdc++.h>
using namespace std;
void Downsample(vector<double> &x, vector<double> &y, int initial_position , int point_num , int N)
{
    int x_index = 0;
    for(int i = 0; i < point_num ; i++)
    {
        y.push_back( x[initial_position + i*N] ) ;
    }
}
