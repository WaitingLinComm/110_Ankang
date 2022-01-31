#include <vector>
#include <iostream>
using namespace std;

void OQPSK_bitReorganizationRx(vector<double> &OutBit, vector<double> &InBit, int L_InBit)
{
    for(int i = 0; i < L_InBit/2; i++){
        OutBit.push_back(InBit[2 * i + 1]);
        OutBit.push_back(InBit[2 * i]);
    }
}