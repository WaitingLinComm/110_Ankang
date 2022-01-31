// Lx > Ly
void overlap_add(int Lb, vector<double> &conv_block_I, vector<double> &conv_block_Q, vector<double> &x_I, vector<double> &x_Q, vector<double> &y_I, vector<double> &y_Q)
{    
    int Lx, Ly, Lz;
    Lx = x_I.size(); Ly = y_I.size(); 
    Lz = Lx + Ly - 1; 
    int Nb = ceil(double(Lx)/double(Lb));       // number of block
    int L_overlap = Ly - 1;     
    int L_bconv =  Ly + Lb - 1; // length of block after convolution with y

    // Create buffer
    vector<double>  x_block_I, x_block_Q; 

    int start_block, end_block;
    int L_nonovelap, start_col_index, end_col_index;
    int First_block_end_col_index;
    for(int block_index = 1; block_index <= Nb ;block_index++){
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
                complex_convolution(z_I, z_Q, Lz, x_block_I, x_block_Q, Lb, y_I, y_Q, Ly);
                
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
}