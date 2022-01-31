#include <vector>
#include <iostream>
using namespace std;

void OQPSK_DecisionProcess(vector<double> &OutBit, int L_OutBit, vector<double> &real_part_Bit, vector<double> &imag_part_Bit, int L_real_part_Bit)
{
    for(int i = 0; i < L_real_part_Bit; i++){
        if(real_part_Bit[i] > 0)
            OutBit.push_back(1);
        else
            OutBit.push_back(0);

        if(imag_part_Bit[i] > 0)
            OutBit.push_back(1);
        else
            OutBit.push_back(0);
    }
}    