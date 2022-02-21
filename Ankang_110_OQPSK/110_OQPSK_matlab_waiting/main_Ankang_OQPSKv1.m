clc,clear,close all
%%  Data Paremeters
% Tx info.
fsy = 0.256*10^6;        % symbol rate  
Tsy = 1/fsy;             % symbol period
% USRP info.
fs_USRP = 6.25e6;        % sampling frequency
Ts_USRP = 1/fs_USRP;     % sampling period
sps_USRP = fs_USRP/fsy;  % SamplePerSymbol. NOTE: USRP_sps is not interger. While sps should be even integer, so Resample Process is needed later on. 
%% Read File
file_name = 'burst2_sr6.25_dtCplx_st8t'; % 110 OQPSK
read_samples = 1e6;
[rawData] = DataConversion(file_name, read_samples);
%% Plot Spectrum of OQPSK data 
% figure
% plotspec(data1,Ts);
%% DC Blocker (Remove DC offset)
Data_withoutDC = rawData - mean(rawData);
% figure
% plotspec(data2,Ts);
%% Down converter
f_c_subcarrier = -338745; 
data_downconverter = downconverter(Data_withoutDC, f_c_subcarrier, Ts_USRP);
% figure
% plotspec(data_demod,Ts);
%% LPF
%[data_LP] = butterworthLPF(3, 1/13, data_demod); % [LP_Sig] = chebyshev(n, Rp, Wp, InSig)
[data_LP] = butterworthLPF(3, 2/39, data_downconverter);
% figure;
% plotspec(data_LP,Ts);
%%  Resample
old_sps = sps_USRP;
new_sps = 26;           % Set by ourselves
N = new_sps / old_sps;  % upsample by N. NOTE: N can be non-integer 
cpu = 8;                % cpu core of computer(used in parfor)
window = 6;             % half-sided window length(used in SincInterpolation)  
[Sig] = resample(data_LP, N, cpu, window);
new_fs = fs_USRP * N;   % new fs
% figure
% plotspec(Sig,Ts);
%%  Pulse Shaping Filter initialization
RolloffFactor = 0.3;
FilterSpanInSymbols = 6;
Pulseshaping = SRRC(new_sps, RolloffFactor, FilterSpanInSymbols);   
%{
figure
stem([-new_sps * FilterSpanInSymbols/2 : new_sps * FilterSpanInSymbols/2], Pulseshaping)
title("Square Root Raised Cosine filter");
figure
plotspec(Pulseshaping, 1/new_fs);
%}
%% Bit sequence generating parameters
%{
% if bitorder = 1: even number of b represents real bit value
%                  odd number of b represents imaginary bit value
% if bitorder = 2: even number of b represents imaginary bit value
%                  odd number of b represents real bit value

% if bitmap = 1: change bit value(1 / 0) to voltage (-1 / 1)
% if bitmap = 2: change bit value(1 / 0) to voltage (1 / -1)

% if IQinterlace = 1: Q leads I
% if IQinterlace = 2: I leads Q
%}
bitorder = 1;
bitmap = 2;
IQinterlace = 1;
%% Get syncSig from pilots
% Let pilotsym  go through the Tx and get the coresponding Tx OTFS waveform(before modulator) 
pilotPattern = '213031330103312010011323310';       % 27 symbols => 54 bits in total (OQPSK pilot symbols form 中心)
[m_p, ~] = symbol_mapping(pilotPattern);            % Transfer the OQPSK pilot symbols to bits % 27*2 = 54 bits in total
[b_p, psi_p, l_p, u_p, syncSig] = OQPSK_frontendTx(pilotPattern, m_p, Pulseshaping, new_sps, bitorder, bitmap, IQinterlace);
% NOTE:
%{
% disp(">>> b_p:");
% fprintf("%d", b_p); fprintf("\n");
%  Using the following symbol mapping with: bitorder = 1; bitmap = 2; IQinterlace = 1, we can get the highest matchfiltering peak from TO esimate 
%  01 | 00       1 | 0
%  ---+---  => ----+----
%  11 | 10       2 | 3
%}
%% Let syncSig go through LPF (syncSig is a lowpass signal, so it is no need for syncSig to go through LPF.)
%{
[syncSig_LP] = butterLPF(3, 2/39, syncSig);  %[syncSig_LP] = butterLPF(3, 1/13, syncSig);
% figure
% plotspec(syncSig_LP,Ts);
%}
%%  Course TO estimation 
[~, y1_norm2, coarse_pilot_position] = coarseTO_estimator(Sig, syncSig, new_sps); % Find the course positon of the pilot
coarse_pilot_position = coarse_pilot_position(9); % First test with burst9: The position of highest peak(r[m_9])  
%figure
%plot(y1_norm2); % stem(MF_amplitude); 
%% Get processed data
K = 3000;
v = Sig(coarse_pilot_position - K : coarse_pilot_position + K);   
% After finding the course position of the pilot, the number of samples of ...
% processed data in each burst for the next stage is 2*K + 1. 
%% CFO, PHO, TO estimation
[cfo_hat, pho_hat, d, energy_buffer, vCFO_buffer] = CFOPHOTO_estimator(v, syncSig, K, 1/new_fs);
% 3D Plot
%{
for i = 1:length(energy_buffer)
    figure
    [m,n] = size(energy_buffer{i});
    mesh([1:n] - length(syncSig) + 1, vCFO_buffer{i}, energy_buffer{i}); % vCFO_buffer{i} (Hz)
    xlabel('signal time index');
    % xticks([0 3000 6000]); xticklabels({-3000,0,3000})
    ylabel('CFO(Hz)');
end
%}
%% Some plots for cfo, pho evaluation 
% CFOPHO_evaluation(K + d, v, syncSig, cfo_hat);
%% Get the exact pilot position
exact_pilot_position = coarse_pilot_position + d; 
%% Reget the correct received pilot and data
analysis_symbols = 300;     
[RegetReceived_pilot, Regetdata_burst] = get_databurst(analysis_symbols, Sig, syncSig, exact_pilot_position, new_sps);
%%  CFO&CPO compensation
Received_pilot_compen =  CFOPHOcompen(RegetReceived_pilot, pho_hat, cfo_hat, 1/new_fs); 
data_burst_compen =  CFOPHOcompen(Regetdata_burst, pho_hat, cfo_hat, 1/new_fs);
%{
%% 2nd CFO estimation (Var method)
cfo_Estimate2 = CFOestimateVarMethod(Sigcompen, syncSig, Ts); % rad
%CFO_PHO_evaluation(3001, Sigcompen2, syncSig, cfo_Estimate2);
%% 2nd PHO estimation
cpo_Estimate2 = PHOestimate2(Sigcompen, syncSig);
Sigcompen2 =  CFOPHOcompen(Sigcompen, cfo_Estimate2, cpo_Estimate2, Ts);
%}
%% OQPSK aftend Rx
[u_hat_p, l_hat_p, b_i_hat_p, b_q_hat_p, RxSym_p] = OQPSK_aftendRx(Received_pilot_compen, Pulseshaping, FilterSpanInSymbols, new_sps, IQinterlace);
[u_hat_d, l_hat_d, b_i_hat_d, b_q_hat_d, RxSym_d] = OQPSK_aftendRx(data_burst_compen, Pulseshaping, FilterSpanInSymbols, new_sps, IQinterlace);
%% Plot l_hat, b_i_hat, b_q_hat (Plot without PHO aftendRx Optimization)
plotmod = 1;
plot_lilq_bibq(plotmod, b_p, l_hat_p, b_i_hat_p, b_q_hat_p);
% plotmod = 2;
% plot_lilq_bibq(plotmod, b_p, l_hat_d, b_i_hat_d, b_q_hat_d);
%% Plot constellation 
plot_constellation(b_i_hat_p, b_q_hat_p); 
%plot_constellation(b_i_hat_d, b_q_hat_d); 
%% DecisionProcess => get b_hat
[b_hat] = DecisionProcess(b_i_hat_p, b_q_hat_p);
%% Reorganize bit order => get m_hat
[m_hat] = bitReorganization(bitorder, b_hat); 
%% Symbol demapping
[ReceivedSym] = symbol_demapping(m_hat);
%% SNR estimation
AllpilotSym = b_i_hat_p + j * b_q_hat_p; 
[SNR_dB] = SNRestimation("OQPSK", AllpilotSym);
%% Calculate Bit Error Rate (by using pilot)
pilotPattern_vec = [2 1 3 0 3 1 3 3 0 1 0 3 3 1 2 0 1 0 0 1 1 3 2 3 3 1 0];
% sol1:
% BER = error_pilot/length(pilotPattern_vec);
% sol2:
[SER, BER, error_pilot] = ErrorRateCalculation("QPSK", pilotPattern_vec, ReceivedSym);
%% PHO aftendRx Optimization
%{
[U] = PHO_aftendRx_Optimization(l_hat_p, l_p);
%% PHO aftendRx Compensation
%k = l_hat_p * U; % PHO compensation (only compensate received pilot)
k = l_hat_p * U; % PHO compensation (compensate all the data)
%% Align I, Q ch, ... and get b_i_hat, b_q_hat 
if IQinterlace == 1             % If Tx: I leads Q    
    b_i_hat = real(k(1:2:end));  
    b_q_hat = imag(k(2:2:end)); 
elseif IQinterlace == 0         % If Tx: Q leads I    
    b_i_hat = real(k(2:2:end));  
    b_q_hat = imag(k(1:2:end)); 
end
%% Plot l_hat, b_i_hat, b_q_hat (Plot without PHO aftendRx Optimization)
plotmod = 1;
plot_lilq_bibq(plotmod, b_p, l_hat_p, b_i_hat, b_q_hat);
%% Plot constellation
plot_constellation(b_i_hat, b_q_hat);
%}