#include <iostream>
#include <fstream>
using namespace std;

void readtxt2array(const char *filename, int filelength, double *num)
{
    //int num[filelength];
    ifstream fin(filename);
    if(!fin) { 
        cout << "Failed to open file.\n";
    }
    for(int i = 0; i < filelength;i++)
        fin >> num[i];
    fin.close();

    /*
    for(int i = 0; i < Filelength; i++){ 
    cout << num[i] << "\n";
    }
    */
}    