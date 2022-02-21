% The function: 
% 1. Calculate the Bit Error Rate of the signal through the received pilots.
% 2. Plot the Calculated Bit Error Rate and the Theratical Bit Error Rate curve
function [error, SNR, BER , SER] = BER_calculation( mode_type , pilot, datapilot_register)
%% Parameter Setting 
% Input:
% mod_type : "QPSK","8PSK"
% pilot : pilot mapping(from spec)
% datapilot_register: a vector of all of the received pilots

% Output: 
% Bit Error Rate of the signal through the received pilots 
%% Divide pilots into different quadrant
if mode_type == "QPSK"
    sp1=[];sp2=[];sp3=[];sp4=[];               % collection of pilots in each quadrant
    sn1=0;sn2=0;sn3=0;sn4=0;                   % number of pilots in each quadrant
    for i=1:length(datapilot_register)
        phase = angle(datapilot_register(i));  % rad
        if phase > 0 & phase <= pi/2           % 1st quadrant
            sp1 = [sp1 datapilot_register(i)];
            sn1 = sn1 + 1;
        elseif phase > pi/2 & phase <= pi      % 2nd quadrant
            sp2 = [sp2 datapilot_register(i)];
            sn2 = sn2 + 1;
        elseif phase > -pi & phase <= -pi/2    % 3rd quadrant
            sp3 = [sp3 datapilot_register(i)];
            sn3 = sn3 + 1;
        else   phase > -pi/2 & phase <= 0      % 4th quadrant
            sp4 = [sp4 datapilot_register(i)];
            sn4 = sn4 + 1;
        end
    %sol2:
    %{
    %     if real(datapilot_register(i))>0 & imag(datapilot_register(i))>0     %1st quadrant
    %         sp1 = [sp1 datapilot_register(i)];
    %         sn1 = sn1 + 1;
    %     elseif real(datapilot_register(i))<0 & imag(datapilot_register(i))>0 %2nd quadrant
    %         sp2 = [sp2 datapilot_register(i)];
    %         sn2 = sn2 + 1;
    %     elseif real(datapilot_register(i))<0 & imag(datapilot_register(i))<0 %3rd quadrant
    %         sp3 = [sp3 datapilot_register(i)];
    %         sn3 = sn3 + 1;
    %     else   real(datapilot_register(i))>0 & imag(datapilot_register(i))<0 %4th quadrant
    %         sp4 = [sp4 datapilot_register(i)];
    %         sn4 = sn4 + 1;
    %     end
    %}
    end
    sp_rotate = [sp1  sp2.*exp(-j*pi/2) sp3.*exp(-j*pi) sp4.*exp(-j*pi*3/2)];

elseif mode_type == "8PSK"
    sp1=[];sp2=[];sp3=[];sp4=[];sp5=[];sp6=[];sp7=[];sp8=[]; % collection of pilots in each quadrant
    sn1=0;sn2=0;sn3=0;sn4=0;sn5=0;sn6=0;sn7=0;sn8=0;         % number of pilots in each quadrant
        for i=1:length(datapilot_register)
            phase = angle(datapilot_register(i));            % rad
            if phase > -pi/8 & phase <= pi/8     
                sp1 = [sp1 datapilot_register(i)];
                sn1 = sn1 + 1;
            elseif phase > pi/8 & phase <= 3*pi/8 
                sp2 = [sp2 datapilot_register(i)];
                sn2 = sn2 + 1;
            elseif  phase > 3*pi/8 & phase <= 5*pi/8 
                sp3 = [sp3 datapilot_register(i)];
                sn3 = sn3 + 1;
            elseif  phase > 5*pi/8 & phase <= 7*pi/8  
                sp4 = [sp4 datapilot_register(i)];
                sn4 = sn4 + 1;
            elseif  phase > 7*pi/8 || phase <= -7*pi/8
                sp5 = [sp5 datapilot_register(i)];
                sn5 = sn5 + 1;
            elseif  phase > -7*pi/8 &  phase <=  -5*pi/8
                sp6 = [sp6 datapilot_register(i)];
                sn6 = sn6 + 1;
            elseif  phase >  -5*pi/8 & phase <= -3*pi/8 
                sp7 = [sp7 datapilot_register(i)];
                sn7 = sn7 + 1;
            elseif  phase >  -3*pi/8 & phase <= -1*pi/8
                sp8 = [sp8 datapilot_register(i)];
                sn8 = sn8 + 1;
            end
        end
        sp_rotate = [sp1  sp2.*exp(-j*pi/4) sp3.*exp(-j*pi/2) sp4.*exp(-j*pi*3/4) sp5.*exp(-j*pi) sp6.*exp(-j*pi*5/4) sp7.*exp(-j*pi*3/2) sp8.*exp(-j*pi*7/4)];
end
sp_rotate_average = sum(sp_rotate)/length(sp_rotate);
%% Calculate pilots' variance (power)
sp_avg_power = sum(abs(sp_rotate).^2)/length(sp_rotate);
% Plot: All the pilots are concentrated to the first quadrant  
% figure
% scatter(real(sp_rotate),imag(sp_rotate),'.'); hold on
% scatter(real(sp_rotate_average),imag(sp_rotate_average),'.','red');
% xlim([-4 4]);
% ylim([-4 4]);

%% Calculate noise's variance (power)
sp_noise_power = sum(abs(sp_rotate - sp_rotate_average).^2)/length(sp_rotate); 

%% Calculate the SNR of the received pilots
SNR = sp_avg_power/sp_noise_power;
r_SNR_dB = 10*log10(SNR);

%% Calculate the BER of the received pilots
if mode_type == "QPSK"
    M = 4; % 4-PSK
    k = log2(M);
    
    pilotb = pskdemod(pilot,4,pi/4); % 0:1+j; 1:-1+j; 2:-1-j; 3:1-j;
    datapilot_registerb = pskdemod(datapilot_register,4,pi/4);
elseif mode_type == "8PSK"
    M = 8; % 8-PSK
    k = log2(M);
    
    pilotb = pskdemod(pilot,8,pi/8); 
    datapilot_registerb = pskdemod(datapilot_register,8,pi/8);
end

error = 0;
for i = 1:length(pilot):length(datapilot_registerb)
    if datapilot_registerb(i) ~= pilotb(mod(i,length(pilot)))
        error = error + 1;
    end    
end
BER = error/(length(datapilot_registerb));
SER = 1/k*BER;
%% Calculate theoratical SNR/BER
SNRdB = [1:16]; %SNR/bit,dB
SNRlin = 10.^(SNRdB/10);

SER_theory = 2*qfunc(sqrt(2*k*SNRlin)*sin(pi/M)); %Proakis 7.6.59 
BER_theory = 1/k.*SER_theory; %Proakis 7.6.60
%% Plot the calculated BER/SER & the theoratical SNR/BER
figure
semilogy(SNRdB,SER_theory,'-.r*'); hold on %plot theory SNR
semilogy(r_SNR_dB,SER,'bo'); %plot SNR of received signal
xlabel('SNR(dB)');
ylabel('SER');
title('Signal SNR & Theoratical SER')
legend('Theoratical SER','Signal SNR')
end