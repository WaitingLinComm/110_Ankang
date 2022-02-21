function [SNR_dB] = SNRestimation(mode_type, received_pilot)
%SNRestimation
%
%   Use pilot to estimate the SNR(Signal to Noise Ratio) of signal
%
%   Input variable: mode_type(a vector), received_pilot(a vector)
%   Output variable: SNR_dB(a scalar; Unit: dB)
%
%   mod_type : "QPSK", "8PSK", "OQPSK"
%
%   SNR = S/N0 % linear scale
%   S: power of received pilot
%   N0: noise power of received pilot
%   SNR_dB = 10 * log10(SNR); % dB

%% Calculate power of received pilot: S
S = norm(received_pilot,2)^2/length(received_pilot);
a = sqrt(S/2);        % scalar of the desired pilot symbol on the constellation set
%% Divide received pilots into different quadrant
if mode_type == "QPSK" || mode_type == "OQPSK"      
    sp1 = []; sp2 = []; sp3 = []; sp4 = []; % collection of pilots in each quadrant
    sn1 = 0; sn2 = 0; sn3 = 0; sn4 = 0;     % number of pilots in each quadrant

    z1 = a * (1 + 1i);    % desired pilot symbol in 1st quadrant
    z2 = a * (-1 + 1i);   % desired pilot symbol in 2nd quadrant
    z3 = a * (-1 - 1i);   % desired pilot symbol in 3rd quadrant
    z4 = a * (1 - 1i);    % desired pilot symbol in 4th quadrant

    for i = 1:length(received_pilot)
        phase = angle(received_pilot(i));   % rad
        if phase > 0 && phase <= pi/2       % 1st quadrant
            sp1 = [sp1 received_pilot(i)];
            sn1 = sn1 + 1;
        elseif phase > pi/2 && phase <= pi  % 2nd quadrant
            sp2 = [sp2 received_pilot(i)];
            sn2 = sn2 + 1;
        elseif phase > -pi && phase <= -pi/2 % 3rd quadrant
            sp3 = [sp3 received_pilot(i)];
            sn3 = sn3 + 1;
        elseif phase > -pi/2 && phase <= 0   % 4th quadrant
            sp4 = [sp4 received_pilot(i)];
            sn4 = sn4 + 1;
        end 
    end
        %% Calculate noise's variance (power)
    N0 = (1 / length(received_pilot)) * (norm(sp1 - z1, 2) ^ 2 + ...
        norm(sp2 - z2, 2) ^ 2 + norm(sp3 - z3, 2) ^ 2  + norm(sp4 - z4, 2) ^ 2 ); 
elseif mode_type == "8PSK"
    sp1 = []; sp2 = []; sp3 = []; sp4 = []; sp5 = []; sp6 = []; sp7 = []; sp8 = []; % collection of pilots in each quadrant
    sn1 = 0; sn2 = 0; sn3 = 0; sn4 = 0; sn5 = 0; sn6 = 0; sn7 = 0; sn8 = 0;         % number of pilots in each quadrant
    z1 = a * (sqrt(2) + 0i);    % desired pilot symbol in 1st quadrant
    z2 = a * (1 + 1i);   % desired pilot symbol in 2nd quadrant
    z3 = a * (0 + sqrt(2)*i);   % desired pilot symbol in 3rd quadrant
    z4 = a * (-1 + 1i);    % desired pilot symbol in 4th quadrant
    z5 = a * (-sqrt(2) + 0);    % desired pilot symbol in 4th quadrant
    z6 = a * (-1 - 1i);    % desired pilot symbol in 4th quadrant
    z7 = a * (0 - sqrt(2)*i);    % desired pilot symbol in 4th quadrant
    z8 = a * (1 - 1i);    % desired pilot symbol in 4th quadrant
    for i = 1:length(received_pilot)
        phase = angle(received_pilot(i));           % rad
        if phase > -pi/8 && phase <= pi/8           % desired pilot symbol in 1st quadrant
            sp1 = [sp1 received_pilot(i)];
            sn1 = sn1 + 1;
        elseif phase > pi/8 && phase <= 3*pi/8       % desired pilot symbol in 2nd quadrant
            sp2 = [sp2 received_pilot(i)];
            sn2 = sn2 + 1;
        elseif  phase > 3*pi/8 && phase <= 5*pi/8    % desired pilot symbol in 3rd quadrant
            sp3 = [sp3 received_pilot(i)];
            sn3 = sn3 + 1;
        elseif  phase > 5*pi/8 && phase <= 7*pi/8    % desired pilot symbol in 4th quadrant
            sp4 = [sp4 received_pilot(i)];
            sn4 = sn4 + 1;
        elseif  phase > 7*pi/8 || phase <= -7*pi/8   % desired pilot symbol in 5th quadrant
            sp5 = [sp5 received_pilot(i)];
            sn5 = sn5 + 1;
        elseif  phase > -7*pi/8 &&  phase <=  -5*pi/8 % desired pilot symbol in 6th quadrant
            sp6 = [sp6 received_pilot(i)];
            sn6 = sn6 + 1;
        elseif  phase >  -5*pi/8 && phase <= -3*pi/8  % desired pilot symbol in 7th quadrant
            sp7 = [sp7 received_pilot(i)];
            sn7 = sn7 + 1;
        elseif  phase >  -3*pi/8 & phase <= -1*pi/8  % desired pilot symbol in 8th quadrant
            sp8 = [sp8 received_pilot(i)];
            sn8 = sn8 + 1;
        end
    end
        %% Calculate noise's variance (power)
        N0 = (1 / length(received_pilot)) * (norm(sp1 - z1, 2) ^ 2 + ...
        norm(sp2 - z2, 2) ^ 2 + norm(sp3 - z3, 2) ^ 2  + norm(sp4 - z4, 2) ^ 2  +...
        norm(sp5 - z5, 2) ^ 2 + norm(sp6 - z6, 2) ^ 2  + norm(sp7 - z7, 2) ^ 2 + norm(sp8 - z8, 2) ^ 2); 
end
%% Calculate the SNR of the received pilots
SNRlin = S / N0;              % linear scale
SNR_dB = 10 * log10(SNRlin);  % dB
end    