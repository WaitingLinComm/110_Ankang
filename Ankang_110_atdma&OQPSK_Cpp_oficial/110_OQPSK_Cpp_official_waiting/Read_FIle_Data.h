#pragma once
#include <stdio.h>
#include <stdlib.h>
//#include <omp.h>
//#include<bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <string>
#include <str2double.h>
#include <str2float.h>
#include <str2int.h>
#include <str2char.h>
using namespace std;

void Read_File_Data(double &fsy , int &resample_point , int &detection_type , double &fs_USRP, int &filelength , const char *MOD_type[]   , const char *filename[] , const char *filename_save , const char *data_parameter , int &FilterSpanInSymbols , float &RolloffFactor)
{

	ifstream file;
	file.open(data_parameter);
	int para_count = 0 ;

	string line

	while( getline(file , line) ){
 

		int point_position = 0 ;
		int start_ind = 0 ;
		int end_index = 0 ;
		string data;


		find_num_ind( line ,  start ,  end ,  point_position );

		data = line.substr( start+1 , end_index - start_index - 1 ) ;

		if ( para_count == 0 ){
			str2double( fsy , data , end_index - start_index  , point_position-start -1 ) ;
		}
		else if ( para_count == 1){
			str2double( fs_USRP , data , end_index - start_index  , point_position-start -1 ) ;
		}
		else if ( para_count == 2){
			str2float( RolloffFactor , data , end_index - start_index  , point_position-start -1 ) ;			
		}
		else if ( para_count == 3){
			str2int( resample_point , data , end_index - start_index ) ;			
		}
		else if ( para_count == 4){
			str2int( detection_type  , data , end_index - start_index  ) ;
		}
		else if ( para_count == 5){
			str2int( filelength  , data , end_index - start_index ) ;
		}
		else if ( para_count == 6){
			str2int( FilterSpanInSymbols , data , end_index - start_index  ) ;
		}
		// else if ( para_count == 7){
		// 	str2char( MOD_type , data , end_index - start_index - 1 ) ;
		// }
		// else if ( para_count == 8){
		// 	str2char( filename , data , end_index - start_index - 1 ) ;
		// }
		// else if ( para_count == 9){
		// 	str2char( filename_save , data , end_index - start_index - 1 ) ;
		// }



		para_count = para_count + 1 ;
	
	}
	fclose(file);
	

}