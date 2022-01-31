/* 
 * Upsample
 *  
 *  Upsample by N(N should be an integer)
 *  input array:  x
 *  output array: y
 *  Length of the output array:L_y
 */
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vector>
//#include<bits/stdc++.h>
using namespace std;
void Upsample(vector<double> &x, vector<double> &y, int N)
{
    int x_index = 0;
    for(int i = 0; i < x.size(); i++)
    {
        y[ i*N ] = x[i];
    }
}