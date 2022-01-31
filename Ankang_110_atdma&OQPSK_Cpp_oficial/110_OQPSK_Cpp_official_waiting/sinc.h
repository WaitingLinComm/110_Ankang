#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define pi 3.14159265358979323846

double sinc(double x)
{
    double y;
    if(x != 0)
    {
        y =  sin(pi * x) / (pi * x);
    }
    else
    {
        y = 1;
    } 
    return y;
}


