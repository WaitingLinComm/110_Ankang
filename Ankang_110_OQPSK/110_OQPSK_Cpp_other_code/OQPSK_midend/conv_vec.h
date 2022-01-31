/* 
 * convolution: realize z = conv(x, y) in matlab
 * 
 *  Input variable:
 *  x: input signal
 *  y: filter
 *  z: filter result
 * 
 *  Lx: length of input signal
 *  Ly: length of filter
 *  Lz: length of filter result
 * 
 *  NOTE: Lz = Lx + Ly - 1
 * 
 */

//#include <stdio.h>
//#include <stdlib.h>
//#include <math.h>
#include <vector>
#include <iostream>
using namespace std;

void conv_vec(vector<double> &x, vector<double> &y, vector<double> &z, int Lx, int Ly, int Lz)
{
	int i, j, i1;
	double tmp;

	for (i = 0; i < Lz; i++)
	{
		i1 = i;
		tmp = 0;
		for (j=0; j < Ly; j++)
		{
			if(i1 >= 0 && i1 < Lx)
				tmp = tmp + (x[i1] * y[j]);

			i1 = i1-1;
			// z[i] = tmp;			
		}
		z.push_back(tmp);
	}

}
