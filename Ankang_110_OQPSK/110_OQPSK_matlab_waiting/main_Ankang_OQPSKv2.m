clc,clear,close all
%%  Data Paremeters
% Tx info.
fsy = 0.256 * 10^6;      % symbol rate(frequency)
Tsy = 1/fsy;             % symbol period
% USRP info.
fs_USRP = 6.25e6;        % sampling rate(frequency)
Ts_USRP = 1/fs_USRP;     % sampling period
sps_USRP = fs_USRP/fsy;  % SamplePerSymbol. NOTE: USRP_sps is not an interger number. While sps should be even integer, so Resample Process is needed later on. 
%% Read File
file_name = 'burst2_sr6.25_dtCplx_st8t'; % 110 OQPSK data from Ankang Center
%read_samples =  12.5 * 1e6; % 1s data % Because fs = 6.25e6 and OQPSK data is complex value, so there are 6.25e6*2 = 12.5 * 1e6 samples for 1s data
read_samples =  1e6;         % 0.08s data 
%read_samples = 1.25 * 1e6;  % 0.1s data

[rawData] = DataConversion(file_name, read_samples);
%% Plot Spectrum of OQPSK data 
% figure
% plotspec(data1,Ts);
%% DC blocker: remove DC offset
Data_withoutDC = rawData - mean(rawData);
% figure
% plotspec(data2,Ts);
%% Down converter
f_c_subcarrier = -338745; % -1010131, -679997, 341796, 0, 677490, 1019287, 1361083
data_demod = downconverter(Data_withoutDC, f_c_subcarrier, Ts_USRP);
% figure
% plotspec(data_demod,Ts);
%% LPF
[data_LP] = butterworthLPF(3, 2/39, data_demod); %[data_LP] = butterLPF(3, 1/13, data_demod); % [LP_Sig] = chebyshev(n, Rp, Wp, InSig)    
% figure;
% plotspec(data_LP,Ts);
%%  Resample
old_sps = sps_USRP;
new_sps = 26;           % Set by ourselves
N = new_sps / old_sps;  % upsample by N. NOTE: m can be a non-integer number
cpu = 8;                % cpu core of computer(used in parfor)
window = 6;             % half-sided window length(used in SincInterpolation)  

[Sig] = resample(data_LP, N, cpu, window);
new_fs = fs_USRP * N; % new fs
% figure
% plotspec(Sig,1/new_fs);
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
pilotPattern = '213031330103312010011323310';              % 27 symbols => 54 bits in total (OQPSK pilot symbols form 中心)
pilotPattern_vec = [2 1 3 0 3 1 3 3 0 1 0 3 3 1 2 0 1 0 0 1 1 3 2 3 3 1 0];
[m_p, pilotSym] = symbol_mapping(pilotPattern);            % Transfer the OQPSK pilot symbols to bits % 27*2 = 54 bits in total
[b_p, psi, l_p, u, syncSig] = OQPSK_frontendTx(pilotPattern, m_p, Pulseshaping, new_sps, bitorder, bitmap, IQinterlace);
% NOTE:
%{
% disp(">>> b_p:");
% fprintf("%d", b_p); fprintf("\n");
% Using the following symbol mapping with: bitorder = 1; bitmap = 2; IQinterlace = 1, we can get the highest matchfiltering peak from TO esimate 
% 01 | 00       1 | 0
% ---+---  => ----+----
% 11 | 10       2 | 3
%}
%% Let syncSig go through LPF (syncSig is a lowpass signal, so it is no need for syncSig to go through LPF.)
%{
[syncSig_LP] = butterLPF(3, 1/13, syncSig); %[syncSig_LP] = butterLPF(3, 2/39, syncSig);
figure
plotspec(syncSig_LP,Ts);
%}
%% coarseTO_estimator 
[~, y1_norm2, burst_interval] = coarseTO_estimator_v2(Sig, syncSig, new_sps); % Find the coarse_pilot_position, m, of the pilot
%{
name = ['Sig_Re.txt'] ;
writematrix(real(Sig).',name);
name = ['Sig_Im.txt'] ;
writematrix(imag(Sig).',name);
name = ['syncSig_Re.txt'] ;
writematrix(real(syncSig).',name);
name = ['syncSig_Im.txt'] ;
writematrix(imag(syncSig).',name);
%}
% coarse_pilot_position = coarse_pilot_position(9); % First test with burst9: The position of highest peak(r[m_9])  
% figure
% plot(y1_norm2); % stem(MF_amplitude);
%% Go through each burst
number_of_bursts = length(burst_interval);
start_burst = burst_interval(1,:); % coarse_pilot_position
end_burst = burst_interval(2,:);

l_hat_d_buffer = {};
b_i_hat_d_buffer = {};
b_q_hat_d_buffer = {};

for i = 1 : number_of_bursts
    %% Get processed data
    K = 2000;
    v = Sig(start_burst(i) - K : start_burst(i) + K);
    % After finding the course position of the pilot, the number of samples of ...
    % processed data in each burst for the next stage is 2*K + 1. 
    %% CFO, PHO, TO estimation
    [cfo_hat, pho_hat, d, energy_buffer, vCFO_buffer] = CFOPHOTO_estimator(v, syncSig, K, Ts_USRP);
    % fprintf("burst %d\n", i);
    % fprintf("cfo_hat: %f\n",cfo_hat);
    % fprintf("pho_hat: %f\n",pho_hat);
    % fprintf("d: %f\n",d);

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
    exact_pilot_position = start_burst(i) + d; 
    %% Reget the correct received pilot and data 
    % MODE 1: let analysis symbol in each burst be different[correspond to the given Ankang OQPSK signal]
    % [analysis_symbols, RegetReceived_pilot, Regetdata_burst] = get_databurst_v2(start_burst(i), end_burst(i), Sig, syncSig, exact_pilot_position, new_sps);       
    % MODE 2: let analysis symbol in each burst be the same[simplified version]
    end_burst = start_burst(i) + K; % or end_burst(i)
    [analysis_symbols, RegetReceived_pilot, Regetdata_burst] = get_databurst_v2(start_burst(i), end_burst, Sig, syncSig, exact_pilot_position, new_sps);    
    %%  CFO&CPO compensation
    Received_pilot_compen =  CFOPHOcompen(RegetReceived_pilot, pho_hat, cfo_hat, 1/new_fs); 
    data_burst_compen =  CFOPHOcompen(Regetdata_burst, pho_hat, cfo_hat, 1/new_fs);
    %% OQPSK aftend Rx
    [u_hat_p, l_hat_p, b_i_hat_p, b_q_hat_p, RxSym_p] = OQPSK_aftendRx(Received_pilot_compen, Pulseshaping, FilterSpanInSymbols, new_sps, IQinterlace);
    [u_hat_d, l_hat_d, b_i_hat_d, b_q_hat_d, RxSym_d] = OQPSK_aftendRx(data_burst_compen, Pulseshaping, FilterSpanInSymbols, new_sps, IQinterlace);
    %% Buffer
    % for pilot
    l_hat_p_buffer(i,:) = l_hat_p;
    b_i_hat_p_buffer(i,:) =  b_i_hat_p;
    b_q_hat_p_buffer(i,:) =  b_q_hat_p;
    
    % for data
    l_hat_d_buffer{i} = l_hat_d;
    b_i_hat_d_buffer{i} =  b_i_hat_d;
    b_q_hat_d_buffer{i} =  b_q_hat_d;
end
%% vectorize data from buffer
% vec_l_hat_p = vec(l_hat_p_buffer);       % a col vector 
% vec_b_i_hat_p = vec(b_i_hat_p_buffer.'); % a col vector
% vec_b_q_hat_p = vec(b_q_hat_p_buffer.'); % a col vector

% For Matlab version after 2021b which does not support vec(.)
L1 = size(l_hat_p_buffer, 1) * size(l_hat_p_buffer, 2);
L2 = size(b_i_hat_p_buffer, 1) * size(b_i_hat_p_buffer, 2);
L3 = size(b_q_hat_p_buffer, 1) * size(b_q_hat_p_buffer, 2);

vec_l_hat_p = reshape(l_hat_p_buffer, L1, 1);     % vec_l_hat_p = vec(l_hat_p_buffer);  % a col vector 
vec_b_i_hat_p = reshape(b_i_hat_p_buffer.', L2, 1); % vec_b_i_hat_p = vec(b_i_hat_p_buffer.'); % a col vector
vec_b_q_hat_p = reshape(b_q_hat_p_buffer.', L3, 1); % vec_b_q_hat_p = vec(b_q_hat_p_buffer.'); % a col vector
%% vectorize data from cell
vec_l_hat_d = vectorizeCell(l_hat_d_buffer);
vec_b_i_hat_d = vectorizeCell(b_i_hat_d_buffer); % a row vector
vec_b_q_hat_d = vectorizeCell(b_q_hat_d_buffer); % a row vector
%% DecisionProcess => get b_hat
[b_hat_p] = DecisionProcess(vec_b_i_hat_p, vec_b_q_hat_p);
[b_hat] = DecisionProcess(vec_b_i_hat_d, vec_b_q_hat_d);
%% Reorganize bit order => get m_hat
[m_hat_p] = bitReorganization(bitorder, b_hat_p); 
[m_hat] = bitReorganization(bitorder, b_hat); 
%% Symbol demapping
[ReceivedSym_p] = symbol_demapping(m_hat_p);
[ReceivedSym] = symbol_demapping(m_hat);
%tic;
%fprintf("All the process is done.\n"); 
%toc;

%% SNR estimation
AllpilotSym = vec_b_i_hat_p + j * vec_b_q_hat_p; % Collect all the received pilot symbols in each burst
[SNR_dB] = SNRestimation("OQPSK", AllpilotSym);
%% Error rate calculation
pilotPattern_vec = [2 1 3 0 3 1 3 3 0 1 0 3 3 1 2 0 1 0 0 1 1 3 2 3 3 1 0];
[SER, BER, error_pilot] = ErrorRateCalculation("OQPSK", pilotPattern_vec, ReceivedSym_p);
%% Theoratical SNR/BER curve
indexSNRdB = [1:16];
[SER_theory, BER_theory] = SERBER_TheoreticalCurve("OQPSK", indexSNRdB);
%% Save file
%name = ['OQPSK_1361083_1.txt'] ;
%writematrix(ReceivedSym,name);
%% Plot =============================================================

%% Plot l_hat, b_i_hat, b_q_hat
% Plot received pilot
plotmod = 2; % Plot received pilots only; plotmod = 2; % Plot all the received data, including received pilots 
plot_lilq_bibq(plotmod, b_p, vec_l_hat_p, vec_b_i_hat_p, vec_b_q_hat_p);

% Plot received pilot + received data
plotmod = 2; % Plot received pilots only; plotmod = 2; % Plot all the received data, including received pilots 
plot_lilq_bibq(plotmod, b_p, vec_l_hat_d, vec_b_i_hat_d, vec_b_q_hat_d);
%% Plot constellation
plot_constellation(vec_b_i_hat_p, vec_b_q_hat_p); % Plot received pilot
plot_constellation(vec_b_i_hat_d, vec_b_q_hat_d); % Plot received pilot + received data

%% Plot Theoratical SNR v.s. SER curve and the SNR point
plotSNR_SER_BER(indexSNRdB, SNR_dB, BER, BER_theory, SER, SER_theory);
