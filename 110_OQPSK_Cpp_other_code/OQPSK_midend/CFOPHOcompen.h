// 
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
// Input:  vector type
// Output: vector type
//
// 
// The code realize: OutSig_I = InSig .* exp(-1i * pho) .* exp(-1i * cfo * (1 : length(InSig))) in matlab code

#include <vector>
#include <iostream>
using namespace std;

void CFOPHOcompen(vector<double> &OutSig_I, vector<double> &OutSig_Q, vector<double> &InSig_I, vector<double> &InSig_Q, int DataLength, double pho, double cfo, double Ts)
{    
    double ff = cfo; //double ff = 2 * pi * fc / fs;
    double coscoff, sincoff;
    double temp_I, temp_Q; 
    for (int i = 0; i < DataLength; i++)
    {
    	// realize: InSig .* exp(-1i * cfo * (1 : length(InSig)))
        coscoff = cos((i + 1) * ff); // NOTE: in matlab: Sig_demod = InSig .* exp(-1i * 2 * pi * Ts * f_c * (1 : length(InSig)));
    	sincoff = sin((i + 1) * ff);

        temp_I = InSig_I[i] * coscoff + InSig_Q[i] * sincoff;
        temp_Q = - InSig_I[i] * sincoff + InSig_Q[i] * coscoff;

        // realize: InSig .* exp(-1i * pho)    
        coscoff = cos(pho);
        sincoff = sin(pho);
        
        OutSig_I.push_back(temp_I * coscoff + temp_Q * sincoff);
        OutSig_Q.push_back( - temp_I * sincoff + temp_Q * coscoff);
    }

}    
