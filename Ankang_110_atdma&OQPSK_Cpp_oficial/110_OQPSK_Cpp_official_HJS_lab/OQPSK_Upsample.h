/* 
 * Upsample
 *  
 *  Upsample by N(N should be an integer)
 *  input array:  x
 *  output array: y
 *  Length of the output array:L_y
 */

#include <stdio.h>
#include <stdlib.h>

void OQPSK_Upsample(vector<double> &x, vector<double> &y, int N, int L_y)
{
    int x_index = 0;
    for(int i = 1; i <= L_y; i++){
        if(i % N == 1){
            x_index += 1;
            y.push_back(x[x_index - 1]); //y[i - 1] = x[x_index - 1];
        }
        else{
            y.push_back(0); //y[i - 1] = 0; 
        }

    }
}