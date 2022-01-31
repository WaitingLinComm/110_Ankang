#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define pi 3.14159265358979323846
using namespace std;



int convert_to_index( double real_part , double imag_part , char MOD_type[]  ){

    if ( strncmp("QPSK", MOD_type, 4) == 0 ){
        if (-pi < atan2(imag_part ,real_part ) &&  atan2(imag_part ,real_part )  <= -pi/2 ){
                return 3 ;
        }
        else if (-pi/2 < atan2(imag_part ,real_part ) &&  atan2(imag_part ,real_part )  <= 0 ){
                return 2 ;
        }
        else if (0 < atan2(imag_part ,real_part ) &&  atan2(imag_part ,real_part )  <= pi/2 ){
                return 0 ;
        }
        else if ( pi/2 < atan2(imag_part ,real_part ) &&  atan2(imag_part ,real_part )  <= pi ){
                return 1 ;
        }
    } 
    else if ( strncmp("8PSK", MOD_type,4) == 0  ){
        if (-7*pi/8 < atan2(imag_part ,real_part ) &&  atan2(imag_part ,real_part )  <= -5*pi/8 ){
                return 7 ;
        }
        else if (-5*pi/8 < atan2(imag_part ,real_part ) &&  atan2(imag_part ,real_part )  <= -3*pi/8 ){
                return 3 ;
        }
        else if (-3*pi/8 < atan2(imag_part ,real_part ) &&  atan2(imag_part ,real_part )  <= -pi/8 ){
                return 2 ;
        }
        else if ( -pi/8 < atan2(imag_part ,real_part ) &&  atan2(imag_part ,real_part )  <= pi/8 ){
                return 0 ;
        }
        else if ( pi/8 < atan2(imag_part ,real_part ) &&  atan2(imag_part ,real_part )  <= 3*pi/8 ){
                return 1 ;
        }
        else if ( 3*pi/8 < atan2(imag_part ,real_part ) &&  atan2(imag_part ,real_part )  <= 5*pi/8 ){
                return 5 ;
        }
        else if ( 5*pi/8  < atan2(imag_part ,real_part ) &&  atan2(imag_part ,real_part )  <= 7*pi/8 ){
                return 4 ;
        }
        else {
                return 6 ;
        }
    }
}

