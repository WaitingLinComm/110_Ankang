function [data] = DataConversion(file_name, read_samples)
%get_data
% 
%   Input variable: file_name(a string)
%   Output variable: data(a vector)
%
%   file_name: name of the signal data file
[fid, message] = fopen(file_name,'r');
Data_from_file = fread(fid, read_samples,'uint8'); % Read data %Data_from_file =fread(fid, 1e6, 'uint8');
fclose(fid);
%% Convert Unsigned Interger to Signed Interger
for i=1:length(Data_from_file)
    if Data_from_file(i) > 128
        Data_from_file(i) = Data_from_file(i) - 256; % 2^8 = 256 
    end
end
%% Convert Data into Complex Form 
b = reshape(Data_from_file, 2, length(Data_from_file)/2);   % seperate real part and imaginary part
data = b(1,:) + 1j * b(2,:);                                % compose real part and imaginary part
end