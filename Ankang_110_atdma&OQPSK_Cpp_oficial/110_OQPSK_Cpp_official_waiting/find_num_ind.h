#pragma once
#include <stdio.h>
#include <stdlib.h>
//#include <omp.h>
//#include<bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void find_num_ind( string str , int &start , int &end , int &point_position)
{
	int count = 0 ;
	for ( int i = 0 ; i < str.size() ; i++ ){
		if ( str[i] == 34 ){
			if (  count == 0 ){
				start = i ;
				count = count + 1;
			}
			else if (  count == 1 ){
				end = i ;
				return;
			}


		}
		if ( count == 1 and str[i] == '.'){
			point_position = i - start;
		}
	}

	return;



}