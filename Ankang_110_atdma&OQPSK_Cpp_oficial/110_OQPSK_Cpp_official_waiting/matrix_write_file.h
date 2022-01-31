#pragma once
#include <stdio.h>
#include <iostream>
#include<sys/time.h>
#include<unistd.h>
#include <vector>
#include <stdlib.h>
//#include<bits/stdc++.h>
//#include <omp.h>
using namespace std;
void matrix_write_file(vector<vector<int>> &processed_symbol_recieved , int burst_num , int data_num , const char *filename){
	
	FILE *atdma_result ;
	atdma_result = fopen(filename,"wb");
	for( int demod_index = 0 ; demod_index < burst_num ; demod_index++ ){
	    for( int data_index = 0 ; data_index < data_num ; data_index++ ){
	    	fprintf(atdma_result, "%d", processed_symbol_recieved[demod_index][data_index]);
	    	// fwrite(  (int*)&processed_symbol_recieved[ demod_index ][ data_index ] , sizeof(int) , 1, atdma_result) ;
	    }
	    fprintf(atdma_result, "\n");
	    //fwrite("\n" , sizeof(int) , 1 , atdma_result) ;
	}
	
    fclose(atdma_result);
} 