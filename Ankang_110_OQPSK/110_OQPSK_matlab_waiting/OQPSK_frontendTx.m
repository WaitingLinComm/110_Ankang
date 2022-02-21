function [b, psi, l, u, oqpskWaveform] = OQPSK_frontendTx(pilotPattern, m, Pulseshaping, sps, bitorder, bitmap, IQinterlace)
%OQPSK_frontendTx
% 
%   Input variable: m(a vector), pilotsym(a vector), Pulseshaping(a vector), sps(a scalar), bitorder(1 or 2), bitmap(1 or 2), IQinterlace(1 or 2)
%   Output variable: b(a vector), psi(a vector), l(a vector), u(a vector), oqpskWaveform(a vector)
%
%   m: pilotPattern after symbol mapping  
%
%   if bitorder = 1: even number of b represents real bit value
%                    odd number of b represents imaginary bit value
%   if bitorder = 2: even number of b represents imaginary bit value
%                  odd number of b represents real bit value
%
%   if bitmap = 1: change bit value(1 / 0) to voltage (-1 / 1)
%   if bitmap = 2: change bit value(1 / 0) to voltage (1 / -1)
%
%   if IQinterlace = 1: Q leads I
%   if IQinterlace = 2: I leads Q

%% Generate transmit bits (Use the case of OQPSK pilot symbols form 中心)
%pilotPattern = '213031330103312010011323310';       % 27 symbols => 54 bits in total (OQPSK pilot symbols form 中心)
%[m, ~] = symbol_mapping(pilotPattern);            % Transfer the OQPSK pilot symbols to bits % 27*2 = 54 bits in total
%% Reorganize bit order
[b] = bitReorganization(bitorder, m);
%% Bit Mapping: Change bit value (1 / 0) to voltage (-1 / 1) or bit value (1 /  0) to voltage (1 / -1)
if bitmap == 1               % bit value (1 / 0) to voltage (-1 / 1)
    psi = b * (-2) + 1;
elseif  bitmap == 2          % bit value (1 /  0) to voltage (1 / -1)
    psi = b * 2 - 1;
end
%% I Q interlace
n = ones(1, length(psi));
if IQinterlace == 1           % Let Q leads I
    n(2:2:end) = j; 
elseif IQinterlace == 2       % Let I leads Q
    n(1:2:end) = j; 
end
l = psi .* n;
%% upsample by sps/2;
u = upsample(l, sps/2);
%% Pulse shaping filter setting (SRRC)    
% RolloffFactor = 0.3;
% FilterSpanInSymbols = 6;
% Pulseshaping = SRRC(sps, RolloffFactor, FilterSpanInSymbols); 
%% Employ pulse shaping filter (Tx) and create OQPSK waveform
oqpskWaveform = conv(u, Pulseshaping); %oqpskWaveform = filter(Pulseshaping, 1, u);
end