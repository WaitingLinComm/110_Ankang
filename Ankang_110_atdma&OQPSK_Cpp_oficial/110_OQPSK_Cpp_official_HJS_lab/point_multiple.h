#pragma once
#include <stdio.h>
#include <stdlib.h>
void point_multiple( double &result_real , double &result_imag , double former_r , double former_i ,double later_r ,double later_i )
{
    result_real = former_r*later_r - former_i*later_i ;
    result_imag = former_r*later_i + former_i*later_r ;

}
