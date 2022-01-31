// // The code implement overlap add method of convolution with "overlap_add.h"
#include <iostream>
using namespace std;
#include <vector>
#include <math.h>
#define pi 3.141592653589793238462643383279502884
#include <fftw3.h>
#include "complex_convolution.h"
#include "overlap_add.h"
int main() 
{ 
    //The code use overlap add method to implement: conv(x,y)
    vector<double>  x_I;
    vector<double>  y_I;

    // test 1
    /*
    // x = [1 2 3 4 1 2 5 2 3]; 
    // y = [1 2 3 4];
    // conv(x,y)
    int Lx = 9;
    int Ly = 3;
    vector<double>  x_Q(Lx,0); // 先假設為純實數的訊號
    vector<double>  y_Q(Ly,0); // 先假設為純實數的訊號

    x_I.reserve(Lx); x_Q.reserve(Lx);  
    y_I.reserve(Ly); y_Q.reserve(Ly); 

    x_I.push_back(1); x_I.push_back(2); x_I.push_back(3);
    x_I.push_back(4); x_I.push_back(1); x_I.push_back(2);
    x_I.push_back(5); x_I.push_back(2); x_I.push_back(3); 
    //x_I.push_back(5); x_I.push_back(6); x_I.push_back(7);   
    
    y_I.push_back(1); y_I.push_back(2); y_I.push_back(3);
    */

    // test 2
    // x = [1 2 3 4 1 2 5 2 3]; 
    // y = [1 2 3 4];
    // conv(x,y)

    int Lx = 9;
    int Ly = 4; //int Ly = 3;
    vector<double>  x_Q(Lx,0); // 先假設為純實數的訊號
    vector<double>  y_Q(Ly,0); // 先假設為純實數的訊號

    x_I.reserve(Lx); x_Q.reserve(Lx);  
    y_I.reserve(Ly); y_Q.reserve(Ly); 

    x_I.push_back(1); x_I.push_back(2); x_I.push_back(3);
    x_I.push_back(4); x_I.push_back(1); x_I.push_back(2);
    x_I.push_back(5); x_I.push_back(2); x_I.push_back(3); 
    //x_I.push_back(5); x_I.push_back(6); x_I.push_back(7);   
    
    y_I.push_back(1); y_I.push_back(2); y_I.push_back(3); y_I.push_back(4);

    ///////////////////////////////////////////////////////////////////////////
    int Lb = 3; // block length
    //int Nb = ceil(Lx/Lb);       // number of block
    int Lz = Lx + Ly - 1;
    vector<double> out_I(Lz,0); vector<double> out_Q(Lz,0); 

    overlap_add(Lb, out_I, out_Q, x_I, x_Q, y_I, y_Q);

    for(int i = 0; i < Lz; i++){
        printf("[%d]: %1f %1f\n", i, out_I[i], out_Q[i]);              
    }
    return 0;
}
