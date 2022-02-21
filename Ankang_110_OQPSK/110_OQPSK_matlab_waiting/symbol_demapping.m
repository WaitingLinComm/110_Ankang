function [demappingSym] = symbol_demapping(demodBit)
%symbol_demapping
%
%   Input variable: demodBit(a vector)
%   Output variable: demappingSym(a vector)
%
%   In the case of Ankang signal, the mapping is not Gray mapping. 
%   The mapping of Ankang signal is as the following
%   01 | 00       1 | 0
%   ---+---  => ----+----
%   11 | 10       2 | 3

demappingSym = [];
for i = 1:2:length(demodBit)
    if (demodBit(i:i+1) == [0 0])
        demappingSym = [demappingSym 0];
    elseif (demodBit(i:i+1) == [0 1])
        demappingSym = [demappingSym 1];
    elseif (demodBit(i:i+1) == [1 1])
        demappingSym = [demappingSym 2];
    elseif (demodBit(i:i+1) == [1 0])
        demappingSym = [demappingSym 3];
    end
end  
end