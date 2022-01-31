// Ref: https://dreamhenry.pixnet.net/blog/post/250472960
/////////////////////////////////////////////////////////
// Read number from .txt to array
/////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include "readtxt2array.h"
using namespace std;

// read a vector
int main(){
    int filelength = 5;
    double num[filelength];
    const char *filename;
    filename = "hello2.txt"; //110 OQPSK
    readtxt2array(filename, filelength, num);


    for(int i = 0; i < filelength; i++){ 
    cout << num[i] << "\n";
    }

    return 0;
}

// read matrix
/*
int main(){
    int num[5][5];
    ifstream fin("hello2.txt");
    if(!fin) { 
        cout << "無法讀入檔案\n"; 
        system("pause");
        return 0;
    }
    for(int i=1;i<=4;i++)
      for(int k=1;k<=4;k++)
        fin >> num[i][k];
    fin.close();
    //system("pause");
    return 0;
}
*/