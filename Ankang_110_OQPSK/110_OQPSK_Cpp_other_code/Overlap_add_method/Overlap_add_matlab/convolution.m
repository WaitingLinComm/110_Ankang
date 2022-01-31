clc,clear,close all

%% Sol1-1: use matlab buit-in function
x = [1 2 3 2];
y = [1 2 1];
Lx = length(x);
Ly = length(y);
Lz = Lx + Ly - 1; % length of the convolution output
z = zeros(1, Lz); % convolution output

z = conv(x, y);

%% Sol1-2: implement explicitly
x = [1 2 3 2];
y = [1 2 1];

Lx = length(x);
Ly = length(y);
Lz = Lx + Ly - 1; % length of the convolution output
z = zeros(1, Lz); % convolution output

for i = 1:Lz
    i1 = i;
    tmp = 0;
    for j = 1:Ly
        if(i1 >= 1 && i1 <= Lx)
            tmp = tmp + (x(i1) * y(j));   
        end    
        i1 = i1-1;
        z(i) = tmp;
    end
end

%% Sol2-1: use matlab buit-in function
x = [1 2 3 2]; % signal
y = [1 2 1];   % filter
Lx = length(x);
Ly = length(y);
Lz = Lx; % NOTE: length of the convolution output = length of signal 
z = zeros(1, Lz); % convolution output

z = filter(y, 1, x); % NOTE: Lx: signal, Ly: filter

%% Sol1-2: implement explicitly
x = [1 2 3 2]; % signal
y = [1 2 1];   % filter

Lx = length(x);
Ly = length(y);
Lz = Lx; % NOTE: length of the convolution output = length of signal 
z = zeros(1, Lz); % convolution output

for i = 1:Lz
    i1 = i;
    tmp = 0;
    for j = 1:Ly
        if(i1 >= 1 && i1 <= Lx)
            tmp = tmp + (x(i1) * y(j));  
        end    
        i1 = i1-1;
        z(i) = tmp;
    end
end
