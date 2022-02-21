function [] = plotSNR_SER_BER(indexSNRdB, SNR_dB, BER, BER_theory, SER, SER_theory)
%plotSNR_SER_BER
%
%   Input variable: indexSNRdB(a vector), SNR_dB(a scalar), BER(a scalar), ...
%                   BER_theory(a vector), SER(a scalar), SER_theory(a vector)
%
%   NOTE:
%   1. The estimation of SER is more precise than BER, so we would like to plot SNR v.s. SER curve
%   2. However, we would plot SNR v.s. BER curve in "OQPSK" because the symbol ... 
%      is not actually meaningful in OQPSK
    
%% plot BER v.s. BER curve and the SNR point
figure
semilogy(indexSNRdB, BER_theory, '-.r*'); hold on %plot theory SNR
semilogy(SNR_dB, BER, 'bo'); % plot SNR and SER of received signal
xlabel('SNR(dB)');
ylabel('BER');
title('Signal SNR & Theoratical BER')
legend('Theoratical Curve','Estimated Result')

%% plot SER v.s. BER curve and the SNR point
figure
semilogy(indexSNRdB, SER_theory, '-.r*'); hold on %plot theory SNR
semilogy(SNR_dB, SER, 'bo'); % plot SNR and SER of received signal
xlabel('SNR(dB)');
ylabel('SER');
title('Signal SNR & Theoratical SER')
legend('Theoratical Curve','Estimated Result')
end    