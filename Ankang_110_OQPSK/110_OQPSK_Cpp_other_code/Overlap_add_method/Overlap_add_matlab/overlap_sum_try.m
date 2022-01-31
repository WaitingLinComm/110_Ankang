clc,clear,close all

%% Sol1-2: implement explicitly
Nb = 3;
Lb = 3; % block length
Ly = 3;
Lx = 9;
Lz = Ly + Lx - 1; %11;
L_overlap = Ly - 1;
L_overlap = 3;

L_bconv =  Ly + Nb - 1; % block after convolution with y
x = zeros(Nb, L_bconv);
x(1,:) = [1 4 10 12 9]; % signal
x(2,:) = [4 9 16 7 6];
x(3,:) = [5 12 22 12 9];

x_row1 = x(1,:); 
x_row2 = x(2,:); 
x_row3 = x(3,:); 

z = zeros(1, Lz); % convolution output

count = 0;
row_index = 0;
t = L_bconv - L_overlap - 1;
for i = 1:Lz
    count = count + 1;
    %row_index = row_index + 1;
    
    tmp = 0;
    row_index = ceil(count/t);
    for j = 1 : row_index    
        tmp = tmp + x(j,i1);
        
    end
    
    z(i) = tmp;
    
    %if(count == L_bconv - L_overlap - 1)
    %    count = 0;
    %end
end
%{
for i = 1:Lz
    i1 = i;
    tmp = 0;
    for j = 1:Nb
        %i1 = i1 + L_overlap; 
        if(i1 >= 1 && i1 <= L_bconv)
 
            tmp = tmp + x(j,i1); %(x(i1) * y(j));  
        end    
        i1 = i1-1;
        z(i) = tmp;
    end
end
z
%}