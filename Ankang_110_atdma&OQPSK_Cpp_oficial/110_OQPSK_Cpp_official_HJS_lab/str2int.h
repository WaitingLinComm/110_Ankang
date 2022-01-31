#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include<bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void str2int( int &int_var , string s , int length )
{

	int_var = 0 ;

	if(s[1] == 45){
		for ( int i = 2 ; i < length ; i++ ){
			int_var = int_var + ( (int)s[i] - 48 ) * pow( 10 , length - i -1) ;
		}
		int_var = - int_var ;
	}
	else{
		for ( int i = 1 ; i < length ; i++ ){
			int_var = int_var + ( (int)s[i] - 48 ) * pow( 10 , length - i -1) ;
		}		

	}
	return;





}