function [SER, BER, error_pilot] = ErrorRateCalculation(mode_type, pilotSym, ReceivedPilotSym)
% ErrorRateCalculation
%
% Use pilot to calculate the error rate of signal
% 
% [Input variable ]: mode_type, pilotSym(a vector), ReceivedPilotSym(a vector)
% [Output variable]: SER(a scalar), BER(a scalar)
% 
% mod_type : "QPSK", "8PSK", "OQPSK"

 if mode_type == "QPSK" || mode_type == "OQPSK"
    M = 4; 
    k = log2(M);
elseif mode_type == "8PSK"
    M = 8; 
    k = log2(M);
end

%% Calculate the BER/SER of the received pilots
error_pilot = 0;
L_pilotSym = length(pilotSym);                % length of pilot symbol: 27
bursts = length(ReceivedPilotSym)/L_pilotSym; % number of burst

for i = 1:bursts
    for j=1:L_pilotSym
        if ReceivedPilotSym((i-1)*L_pilotSym + j) ~= pilotSym(j)
            error_pilot = error_pilot + 1;
        end  
    end
end  

SER = error_pilot/length(ReceivedPilotSym);
BER = k * SER;
end    