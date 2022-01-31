/* 
 * bitReorganization
 * 
 * Input variable: bitorder(1 or 2), L(a scalar), InBit(a vector)
 * Output variable: OutBit(a vector)
 * 
 * if bitorder = 1: even number of b represents real bit value
 *                  odd number of b represents imaginary bit value
 * if bitorder = 2: even number of b represents imaginary bit value
 *                  odd number of b represents real bit value
 */

void OQPSK_bitReorganization(int bitorder, vector<double> &InBit, vector<double> &OutBit, int Pattern_Length)
{
    if(bitorder == 1){   
        for(int i = 0; i <  Pattern_Length/2; i++){
            //OutBit[2 * i] = InBit[2 * i + 1];
            //OutBit[2 * i + 1] = InBit[2 * i];
            OutBit.push_back(InBit[2 * i + 1]);
            OutBit.push_back(InBit[2 * i]);
        }
    }
    
    else if(bitorder == 2)     {
        for(int i = 0; i <  Pattern_Length/2; i++){
            //OutBit[2 * i] = InBit[2 * i];
            //OutBit[2 * i + 1] = InBit[2 * i + 1];
            OutBit.push_back(InBit[2 * i]);
            OutBit.push_back(InBit[2 * i + 1]);
        }
    }
}