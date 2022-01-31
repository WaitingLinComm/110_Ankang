#pragma once
#include <stdio.h>
#include <stdlib.h>
double point_divide_imag( double top_r , double top_i ,double down_r ,double down_i )
{
    double result = 0;
    result = (top_i*down_r - top_r*down_i) / ( down_r*down_r + down_i*down_i ) ;
    return result ;
}