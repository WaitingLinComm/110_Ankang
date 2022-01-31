% Overlap Add method for Linear Convolution
% The code can work propriately
% First create conv_buffer [size: zeros(Nb, Lz)] and add the block convolution result together
% Shortcoming: a large memory space is needed to be created 

close All
clear All
clc

x = [1 2 3 4 1 2 5 2 3]; 
y = [1 2 3];
%x = [1 1 1 1 1 1 1 1 1];
%y = [1 1 1];

Lb = 3; % block length

Lx = length(x);
Ly = length(y);
Nb = ceil(Lx/Lb);       % number of block
Lz = Lx + Ly - 1;
L_overlap = Ly - 1;     % L_overlap = 2
L_bconv =  Ly + Lb - 1; % length of block after convolution with y

% Create buffer
x_block = zeros(1, Lb);
conv_buffer = zeros(Nb, Lz);
conv_result = zeros(1, Lz);

for block_index = 1 : Nb
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
         conv_buffer(block_index, start_col_index : end_col_index) = block_conv_result;
         
         % Add the outcome together
         for i = 1 : Lz
             sum = 0;
             for j = 1 : Nb
                sum = sum + conv_buffer(j,i);
             end
             conv_result(i) = sum;
         end
    
    elseif (mod(Lx, Lb) ~= 0) 
        % Divide x into different block
        if (block_index == 1)
            start_block = 1;
            end_block = start_block + mod(Lx, Lb) - 1;
            x_block = x(start_block : end_block);
        elseif (block_index ~= 1)
            start_block = mod(Lx, Lb) + (Lb - 1) * (block_index - 2) + (block_index - 1);
            end_block = start_block + Lb - 1;
            x_block = x(start_block : end_block);
        end
        
        % Do convolution of each block with y
        block_conv_result = conv(x_block, y);
        
        % Save convolution result into buffer
        if (block_index == 1)
            start_col_index = 1;
            end_col_index = start_col_index + (mod(Lx, Lb) + Ly - 1) - 1;
            conv_buffer(block_index, start_col_index : end_col_index) = block_conv_result;
            
            First_block_end_col_index = end_col_index;
         elseif(block_index ~= 1)   
            L_nonovelap = L_bconv - L_overlap; 
            start_col_index = (First_block_end_col_index - L_overlap + 1) + L_nonovelap * (block_index - 2);
            end_col_index = start_col_index + L_bconv - 1;
            conv_buffer(block_index, start_col_index : end_col_index) = block_conv_result;
        end
        
         % Add the outcome together
         for i = 1 : Lz
             sum = 0;
             for j = 1 : Nb
                sum = sum + conv_buffer(j,i);
             end
             conv_result(i) = sum;
         end
    end  
end
conv_result
