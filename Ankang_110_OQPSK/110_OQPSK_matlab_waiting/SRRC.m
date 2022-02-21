function [SRRC_normalized] = SRRC(sps, RolloffFactor, FilterSpanInSymbols)
%SRRC
% 
%   Input variable: sps(a scalar),  RolloffFactor(a scalar), FilterSpanInSymbols(a scalar)
%   Output variable: SRRC_normalized(a vector)
%
%   sps: SamplePerSymbol

SRRC = rcosdesign(RolloffFactor, FilterSpanInSymbols, sps, 'sqrt'); % length(SRRC) = sps* FilterSpanInSymbols + 1 
SRRC_normalized = SRRC / max(SRRC);
end

