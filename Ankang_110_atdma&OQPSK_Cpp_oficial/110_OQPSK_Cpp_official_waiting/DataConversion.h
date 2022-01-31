#pragma once
#include <stdio.h>
#include <stdlib.h>
//#include <omp.h>
//#include<bits/stdc++.h>
using namespace std;

void DataConversion(const char *filename, int filelength, vector<int> &rawDataI, vector<int> &rawDataQ )
{
	FILE *file;
	int ch = 0;
	int count_data = 0;
	file = fopen(filename, "rb");
	if (file == NULL) 
	{
		printf("ERROR \n");
		return;
	}
	for (count_data = 0 ; count_data < filelength ; count_data=count_data+2)
	{
		fread(&rawDataI[count_data/2], sizeof(char), 1, file);
		fread(&rawDataQ[count_data/2], sizeof(char), 1, file);		
		// Convert Unsigned Interger to Signed Interger
		if (rawDataI[count_data/2] > 128)
		{
			rawDataI[count_data/2] = rawDataI[count_data/2] - 256;
		}
		if (rawDataQ[count_data/2] > 128)
		{
			rawDataQ[count_data/2] = rawDataQ[count_data/2] - 256;
		}
	}
	fclose(file);
	

}
