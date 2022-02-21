function [conv_result] = overlapadd_v1(x, y, Lb)
% overlapadd_v1 
%
% The code use Overlap Add method to realize conv(x,y).
% 
% [Input variable ]: x(a vector), y(a vector), Lb(a scalar)
% [Output variable]: conv_result(a vector)
% Output vector length = Lx + Ly - 1
% 
% Lb: length of block

Lx = length(x);
Ly = length(y);
Nb = ceil(Lx/Lb);       % number of block
Lz = Lx + Ly - 1;
L_overlap = Ly - 1;     % L_overlap = 2
L_bconv =  Ly + Lb - 1; % length of block after convolution with y

% Create buffer
x_block = zeros(1, Lb);
conv_result = zeros(1, Lz);

for block_index = 1 : Nb
    conv_block = zeros(1, Lz);
    if (mod(Lx, Lb) == 0) 
        % Divide x into different block
        start_block = 1 + (block_index - 1) * Lb;
        end_block = start_block + Lb - 1;
        x_block = x(start_block : end_block);
        
         % Convolute each block with y
         block_conv_result = conv(x_block, y);
         
         % Save convolution result into buffer
         L_nonovelap = L_bconv - L_overlap; % L_nonovelap = 3
         start_col_index = 1 + L_nonovelap * (block_index - 1);
         end_col_index = start_col_index + L_bconv - 1;
         conv_block(start_col_index : end_col_index) = block_conv_result; 
         conv_result = conv_result + conv_block;
    
    elseif (mod(Lx, Lb) ~= 0) 
        % Divide x into different block
        if (block_index == 1)
            start_block = 1;
            end_block = start_block + mod(Lx, Lb) - 1;
            x_block = x(start_block : end_block);
            
            % Do convolution of each block with y
            block_conv_result = conv(x_block, y);
            
            % Save convolution result into buffer
            start_col_index = 1;
            end_col_index = start_col_index + (mod(Lx, Lb) + Ly - 1) - 1;
            conv_block(start_col_index : end_col_index) = block_conv_result; 
            conv_result = conv_result + conv_block;
            
            First_block_end_col_index = end_col_index;
        elseif (block_index ~= 1)
            start_block = mod(Lx, Lb) + (Lb - 1) * (block_index - 2) + (block_index - 1);
            end_block = start_block + Lb - 1;
            x_block = x(start_block : end_block);
            
            % Do convolution of each block with y
            block_conv_result = conv(x_block, y);
            
            % Save convolution result into buffer
             L_nonovelap = L_bconv - L_overlap; 
            start_col_index = (First_block_end_col_index - L_overlap + 1) + L_nonovelap * (block_index - 2);
            end_col_index = start_col_index + L_bconv - 1;
            conv_block(start_col_index : end_col_index) = block_conv_result; 
            conv_result = conv_result + conv_block;
        end
    end 
end
