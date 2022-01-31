void DataConversion(const char *filename, int filelength, vector<double> &rawDataI, vector<double> &rawDataQ)
{
	FILE *file;
	int *Data_from_file = (int*)malloc(filelength * sizeof(int));
	int ch = 0;
	int count_data = 0;
	file = fopen(filename, "rb");
	if (file == NULL) 
	{
		printf("ERROR \n");
		return;
	}

	while (count_data < filelength)
	{
		fread(&ch, sizeof(char), 1, file);
		Data_from_file[count_data] = ch;
			
		// Convert Unsigned Interger to Signed Interger
		if (Data_from_file[count_data] > 128)
		{
			Data_from_file[count_data] = Data_from_file[count_data] - 256;
		}
		count_data++;
	}
	fclose(file);
	
	// Save Data into Complex Form 
	for (int i = 0; i < filelength/2; i++)
	{
		rawDataI.push_back(Data_from_file[2*i]); 
		rawDataQ.push_back(Data_from_file[2*i + 1]);
	}
	free(Data_from_file);
}
