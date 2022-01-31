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

[conv_result] = overlapadd_v1(x, y, Lb);

conv_result