#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
//#include<bits/stdc++.h>
using namespace std;
void Print_vector(vector<double> &x )
{
    int x_len = x.size();
    for(int i = 0; i < x_len ; i++)
    {
        cout << x[i] ;
    }
}
