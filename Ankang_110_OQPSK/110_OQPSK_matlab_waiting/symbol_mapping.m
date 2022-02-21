function [pilotBit, pilotSym] = symbol_mapping(pilotPattern)
%symbol_mapping
% 
%   Input variable: pilotPattern(a vector)
%   Output variable: pilotBit(a vector), pilotSym(a vector)
%
%   pilotPattern: 0 or 1 or 2 or 3
%
%   For Gray mapping
%    1 |  0       01 | 00
%   ---+---  =>  ----+----
%    3 |  2       11 | 10
%
%   In the case of Ankang signal, the mapping is not Gray mapping. 
%   The mapping of Ankang signal is as the following
%    1 |  0       01 | 00
%   ---+---  =>  ----+----
%    2 |  3       10 | 11
%
%   pilotBit: pilot bit sequence
%   pilotSym: pilot symbol sequence

pilotBit = [];
pilotSym = [];
for i = 1:length(pilotPattern)
    if (pilotPattern(i) == '0')
        pilotBit = [pilotBit 0 0];
        pilotSym = [pilotSym 1 + 1i];
    elseif (pilotPattern(i) == '1')
        pilotBit = [pilotBit 0 1];
        pilotSym = [pilotSym -1 + 1i];
    elseif (pilotPattern(i) == '2')
        pilotBit = [pilotBit 1 1];
        pilotSym = [pilotSym -1 - 1i];
    elseif (pilotPattern(i) == '3')
        pilotBit = [pilotBit 1 0];
        pilotSym = [pilotSym 1 - 1i];
    end
end
end
