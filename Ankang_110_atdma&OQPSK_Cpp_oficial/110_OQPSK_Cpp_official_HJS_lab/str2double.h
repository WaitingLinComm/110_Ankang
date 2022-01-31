#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include<bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
using namespace std;

void str2double(double &double_var , string s , int length , int point_position)
{
	double_var = 0 ; 

	if( s[1] == 45 ){ // negative
		for ( int i = 2 ; i < point_position ; i++ ){
			double_var = double_var + (s[i] - 48 ) * pow( 10 , point_position - i -1 ) ;
		}

		for ( int i = point_position+1 ; i < length ; i++ ){
			double_var = double_var + (s[i] - 48 ) * pow( 10 , point_position - i ) ;
		}	
		double_var = - double_var ;
	}
	else{
		for ( int i = 1 ; i < point_position ; i++ ){
			double_var = double_var + (s[i] - 48 ) * pow( 10 , point_position - i -1 ) ;
		}

		for ( int i = point_position+1 ; i < length ; i++ ){
			double_var = double_var + (s[i] - 48 ) * pow( 10 , point_position - i ) ;
		}
	}


	return;








}