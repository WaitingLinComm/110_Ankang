% Overlap Add method for Linear Convolution
% 
close All
clear All
clc

x = [1 2 3 4 1 2 5 2 3]; 
y = [1 2 3];
%x = [1 1 1 1 1 1 1 1 1];
%y = [1 1 1];

Lx = length(x);
Ly = length(y);
Lb = 3; % block length
Nb = ceil(Lx/Lb); % number of block

% create buffer
x_block = zeros(1, Lb);
overlap_conv_result = zeros(1, Lx + Ly - 1);
if (mod(Lx, Lb) == 0) 
    conv_buffer = zeros(Nb, Lb + Ly - 1);
elseif(mod(Lx, Lb) ~= 0) 
    conv_buffer = zeros(Nb, Lb + Ly - 1);
end

block_index = 1;
for block_index = 1 : Nb
    % divide x into blocks
    if (mod(Lx, Lb) == 0) 
        start_block = 1 + (block_index - 1) * Lb;
        end_block = start_block + Lb - 1;
        x_block = x(start_block : end_block);
        
         % Do convolution of each block with y
         conv_result = conv(x_block, y);
         
         % save convolution result into buffer
         conv_buffer(block_index,:) = conv_result;
         
         % add the outcome together
         %overlap = L_y - 1;
         %for i = 1 : 
         %end
    elseif (mod(Lx, Lb) ~= 0) 
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
        conv_result = conv(x_block, y);
        
        % save convolution result into buffer
        if (block_index == 1)
            conv_buffer(1, 1:mod(Lx, Lb) + Ly - 1) = conv_result;
        elseif(block_index ~= 1)
             conv_buffer(block_index,:) = conv_result;
        end
    end    
   
    
    
end

%z = conv(x, y)

