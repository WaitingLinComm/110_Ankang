#include <stdio.h>
#include <iostream>
#include<sys/time.h>
#include<unistd.h>
#include <vector>
#include <stdlib.h>
//#include<bits/stdc++.h>
//#include <omp.h>
using namespace std;
void matrix_write_file(vector<vector<int>> &processed_symbol_recieved , int burst_num , int data_num , char *filename){
	
	FILE *atdma_result ;
	atdma_result = fopen(filename,"w");
	
	for( int demod_index = 0 ; demod_index < burst_num ; demod_index++ ){
	    for( int data_index = 0 ; data_index < data_num ; data_index++ ){
	    	fwrite(processed_symbol_recieved[ demod_index ][ data_index ] , sizeof(int) , 1 , atdma_result) ;
	    }
	    fwrite("\n" , sizeof(int) , 1 , atdma_result) ;
	}
} 