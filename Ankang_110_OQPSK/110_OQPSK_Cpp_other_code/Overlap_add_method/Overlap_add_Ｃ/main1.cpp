// The code implement overlap add method of convolution. 
#include <iostream>
using namespace std;
#include <vector>
#include <math.h>
#define pi 3.141592653589793238462643383279502884
#include <fftw3.h>
#include "complex_convolution.h"
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
    int Lb = 4; // block length
    int Nb = ceil(double(Lx)/double(Lb));       // number of block
    int Lz = Lx + Ly - 1;

    int L_overlap = Ly - 1;     // L_overlap = 2
    int L_bconv =  Ly + Lb - 1; // length of block after convolution with y

    // Create buffer
    vector<double>  x_block_I, x_block_Q; 
    vector<double> conv_result_I, conv_result_Q;

    conv_result_I.reserve(Lz); conv_result_Q.reserve(Lz);

    int start_block, end_block;
    int L_nonovelap, start_col_index, end_col_index;
    int First_block_end_col_index;
    //Nb = 2;
    vector<double>  conv_block_I(Lz,0), conv_block_Q(Lz,0); 
    for(int block_index = 1; block_index <= Nb ;block_index++){
        //vector<double>  tmp_conv_block_I(Lz,0), tmp_conv_block_Q(Lz,0); 
        if (fmod(Lx, Lb) == 0) {
            // Divide x into different block
            start_block = 1 + (block_index - 1) * Lb;
            end_block = start_block + Lb - 1;
            
            x_block_I.reserve(Lb); x_block_Q.reserve(Lb); 
            for(int i = start_block - 1; i <= end_block - 1; i++){
                x_block_I.push_back(x_I[i]); x_block_Q.push_back(x_Q[i]);
            }
            // Convolute each block with y
            vector<double>  z_I, z_Q;
            z_I.reserve(Lz); z_Q.reserve(Lz); 
            complex_convolution(z_I, z_Q, Lz ,y_I, y_Q, Ly, x_block_I, x_block_Q, Lb);
            // Save convolution result into buffer
            L_nonovelap = L_bconv - L_overlap; // L_nonovelap = 3
            start_col_index = 1 + L_nonovelap * (block_index - 1);
            end_col_index = start_col_index + L_bconv - 1;

            int count = 0;
            for(int i = start_col_index; i <= end_col_index; i++){
                conv_block_I[i-1] = conv_block_I[i-1] + z_I[count];
                conv_block_Q[i-1] = conv_block_Q[i-1] + z_Q[count];
                count = count + 1;
            }
            z_I.clear(); z_Q.clear();
            x_block_I.clear(); x_block_Q.clear();
        }
        else if(fmod(Lx, Lb) != 0){
            // Divide x into different block
            if (block_index == 1){
                start_block = 1;
                end_block = start_block + fmod(Lx, Lb) - 1;
                
                x_block_I.reserve(Lb); x_block_Q.reserve(Lb); 
                    for(int i = start_block - 1; i <= end_block - 1; i++){
                        x_block_I.push_back(x_I[i]); x_block_Q.push_back(x_Q[i]);
                    } // end for
                // Convolute each block with y
                vector<double>  z_I, z_Q;
                z_I.reserve(Lz); z_Q.reserve(Lz); 
                complex_convolution(z_I, z_Q, Lz ,y_I, y_Q, Ly, x_block_I, x_block_Q, Lb);
                // Save convolution result into buffer
                start_col_index = 1;
                end_col_index = start_col_index + (fmod(Lx,Lb) + Ly - 1) - 1;
                
                int count = 0;
                for(int i = start_col_index; i <= end_col_index; i++){
                    conv_block_I[i-1] = conv_block_I[i-1] + z_I[count];
                    conv_block_Q[i-1] = conv_block_Q[i-1] + z_Q[count];
                    count = count + 1;
                }
                z_I.clear(); z_Q.clear();
                x_block_I.clear(); x_block_Q.clear();
                
                First_block_end_col_index = end_col_index;
            } // end if
            else if((block_index != 1)){
                start_block = fmod(Lx,Lb) + (Lb - 1) * (block_index - 2) + (block_index - 1);
                end_block = start_block + Lb - 1;

                x_block_I.reserve(Lb); x_block_Q.reserve(Lb); 
                    for(int i = start_block - 1; i <= end_block - 1; i++){
                        x_block_I.push_back(x_I[i]); x_block_Q.push_back(x_Q[i]);
                    } // end for
                // Convolute each block with y
                vector<double>  z_I, z_Q;
                z_I.reserve(Lz); z_Q.reserve(Lz); 
                complex_convolution(z_I, z_Q, Lz ,y_I, y_Q, Ly, x_block_I, x_block_Q, Lb);
                
                // Save convolution result into buffer
                L_nonovelap = L_bconv - L_overlap; 
                start_col_index = (First_block_end_col_index - L_overlap + 1) + L_nonovelap * (block_index - 2);
                end_col_index = start_col_index + L_bconv - 1;
                
                int count = 0;
                for(int i = start_col_index; i <= end_col_index; i++){
                    conv_block_I[i-1] = conv_block_I[i-1] + z_I[count];
                    conv_block_Q[i-1] = conv_block_Q[i-1] + z_Q[count];
                    count = count + 1;
                }
                z_I.clear(); z_Q.clear();
                x_block_I.clear(); x_block_Q.clear();
                               
            }
            
        }
    }
                    
            for(int i = 0; i < Lz; i++){
                printf("[%d]: %1f %1f\n", i, conv_block_I[i], conv_block_Q[i]);              
            }
            //cout << conv_block_I.size() <<"\n";
        //   for(int i = 0; i < 10; i++){
        //     printf("[%d]: %1f\n", i, x_block[i]);              
        // }
        
    return 0;
}
