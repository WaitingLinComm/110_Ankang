function [SigCFOCPOTOcompen] = CFOPHOcompen(InSig, pho, cfo, Ts)
% CFOPHOcompen.m
%
% [Input variable ]: InSig(a vector), pho(a scalar), cfo(a scalar), Ts(a scalar)
% [Output variable]: SigCFOCPOTOcompen(a vector)
%
% pho(rad), cfo(rad) : [-pi, pi]

SigCFOCPOTOcompen = InSig .* exp(-1j * pho) .* exp(-1i * cfo * (1 : length(InSig)));
end
