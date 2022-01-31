/*
#include <vector>
#include <iostream>
using namespace std;
*/
void OQPSK_symbol_demapping(vector<double> &demappingSym, int L_demappingSym, vector<double> &demodBit)
{
    
    for(int i = 0; i < L_demappingSym; i++){
        if(demodBit[2 * i] == 0 && demodBit[2 * i + 1] == 0)
            demappingSym.push_back(0);
        if(demodBit[2 * i] == 0 && demodBit[2 * i + 1] == 1)
            demappingSym.push_back(1);
        if(demodBit[2 * i] == 1 && demodBit[2 * i + 1] == 1)
            demappingSym.push_back(2);            
        if(demodBit[2 * i] == 1 && demodBit[2 * i + 1] == 0)
            demappingSym.push_back(3);           
    }    
}