#pragma once
#include <stdio.h>
#include <stdlib.h>
double point_multiple_imag( double former_r , double former_i ,double later_r ,double later_i )
{
    double result = 0;
    result = former_r*later_i + former_i*later_r ;
    return result ;
}