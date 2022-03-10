function y = fftconv(x, h)

%% 先把輸入整型一下 (to column vector)
[dim_x, dim_y] = size(x);
if(dim_y > dim_x)
    x = x.';
end

[dim_x, dim_y] = size(h);
if(dim_y > dim_x)
    h = h.';
end

if(length(h) > length(x))
    tmp = h;
    h = x;
    x = tmp;
    clear tmp;
end

% parameter of conv-FFT
len_y = length(h) + length(x) - 1;
y = zeros(len_y, 1);

fft_size = 2^ceil(log2(len_y));
if(fft_size > 2^21)
    fft_size = 2^21;
    if(fft_size < length(h))
        fprintf('Too Large input...\n');
    end
end

processing_x_block_size = fft_size +1 - length(h);
h_freq = fft(h, fft_size);
clear h;
%% 算 FFT 喔~~~
x_index_begin = 1;

while x_index_begin < length(x)
    x_index_end = x_index_begin + processing_x_block_size - 1;
    if(x_index_end > length(x))
        x_index_end = length(x);
    end
    tmp = ifft( ...
        fft(x(x_index_begin:x_index_end), fft_size) .* ...
        h_freq);

    % 把 output 加上去
    y_index_begin = x_index_begin;
    y_index_end = x_index_begin+length(tmp)-1;

    if(y_index_end > length(y))
        y_index_end = length(y);
    end

    len_valid_output = y_index_end - y_index_begin + 1;

    y(y_index_begin:y_index_end) = ...
        y(y_index_begin:y_index_end) + tmp(1:len_valid_output);

    x_index_begin = x_index_begin + processing_x_block_size;
end