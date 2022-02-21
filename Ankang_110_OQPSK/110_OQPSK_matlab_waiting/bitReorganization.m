function [OutBit] = bitReorganization(bitorder, InBit)
% bitReorganize
% 
% [Input variable ]: bitorder(1 or 2), L(a scalar), InBit(a vector)
% [Output variable]: OutBit(a vector)
% 
% if bitorder = 1: even number of b represents real bit value
%                  odd number of b represents imaginary bit value
% if bitorder = 2: even number of b represents imaginary bit value
%                  odd number of b represents real bit value

OutBit = zeros(1, length(InBit));
if bitorder == 1
    OutBit(2:2:end) = InBit(1:2:end);
    OutBit(1:2:end) = InBit(2:2:end);
elseif bitorder == 2     
    OutBit(2:2:end) = InBit(2:2:end);
    OutBit(1:2:end) = InBit(1:2:end);
end
end

