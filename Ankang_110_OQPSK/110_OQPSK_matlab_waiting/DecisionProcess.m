function [OutBit] = DecisionProcess(real_part_Bit, imaginary_part_Bit)
%DecisionProcess
% 
%   Input variable: real_part_Bit(a vector), imaginary_part_Bit(a vector)
%   Output variable: OutBit(a vector)
OutBit = zeros(1, length(real_part_Bit) + length(imaginary_part_Bit));
OutBit(1:2:end) = real_part_Bit > 0;
OutBit(2:2:end) = imaginary_part_Bit > 0;
end


