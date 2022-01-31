#include <vector>
#include <iostream>
using namespace std;
#include <stdio.h>
#include <stdlib.h>
//#include <math.h>

int main()
{
    
    int N = 3;
    double x[3] = {1,2,3};
    double y[3] = {1,2,4};
    double z[3];
    int Lx = N;
    int Ly = N;
    int Lz = N;


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
            z[i] = tmp;	
            //z.push_back(tmp); // z[i] = tmp;			
        }
    }
    
    for (int i = 0; i < 3; i++){
        printf("%f\n", z[i]);
    }

    return 0;
}