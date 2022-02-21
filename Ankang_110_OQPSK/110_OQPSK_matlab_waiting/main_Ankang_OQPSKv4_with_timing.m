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

tStart1 = tic;
[rawData] = DataConversion(file_name, read_samples);
tElapsed1 = toc(tStart1);
%fprintf("Data conversion is done in %f(sec).\n", tElapsed1);  
%% Plot Spectrum of OQPSK data 
% figure
% plotspec(data1,Ts);
%% DC blocker: remove DC offset
totalElapsed_start = tic; % Start counting the total processed time from here! The time-consuming of DataConversion don't need to count in.
tStart2 = tic;
Data_withoutDC = rawData - mean(rawData);
tElapsed2 = toc(tStart2);
%fprintf("DC offset of data is removed in %f(sec).\n", tElapsed2);  
% figure
% plotspec(data2,Ts);
%% Down converter
f_c_subcarrier = -338745; % -1010131, -679997, 341796, 0, 677490, 1019287, 1361083
tStart3 = tic;
data_demod = downconverter(Data_withoutDC, f_c_subcarrier, Ts_USRP);
tElapsed3 = toc(tStart3);
%fprintf("Down conversion is done in %f(sec).\n", tElapsed3);  
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
window = 1;             %window = 6; % half-sided window length(used in SincInterpolation)  

tStart4 = tic;
[Sig] = resample(data_LP, N, cpu, window);
tElapsed4 = toc(tStart4);
%fprintf("Data resample is done in %f(sec).\n", tElapsed4);  

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
% There are 5 kinds of pilots in OQPSK. % Type 5 is the correct pilot of this data. 
pilotPattern = '213031330103312010011323310'; % type 5 % avg_power = 50 % 27 symbols => 54 bits in total (OQPSK pilot symbols form 中心)
%pilotPattern = '203032102200023132132211222'; % type 1 
%pilotPattern = '021211331131101310000232201112322102300230121203'; % type 2 
%pilotPattern = '231013110301132030033121130'; % type 3 % avg_power = 25
%pilotPattern = '203320323203011130322301302'; % type 4 % avg_power = 30

L_pilotPattern = 27;
tStart5 = tic;
[m_p, pilotSym] = symbol_mapping(pilotPattern);            % Transfer the OQPSK pilot symbols to bits % 27*2 = 54 bits in total
[b_p, psi, l_p, u, syncSig] = OQPSK_frontendTx(pilotPattern, m_p, Pulseshaping, new_sps, bitorder, bitmap, IQinterlace);
tElapsed5 = toc(tStart5);
%fprintf("OQPSK frontendTx process is done in %f(sec).\n", tElapsed5);  
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
% Before implement coarseTO_estimator, 
% we found that we can downsample  signal(:) and pilot(:) by 4 before calculating the above convolution. 
% And then multiply  the position we find  by 4 would become the coarse pilot position.
% Although there are some errors in the found coarse pilot position, we can correct the errors by the later CFOPHOTO_estimator
tStart6 = tic;
down_sample = 5;
Sig_sample = Sig(1:down_sample:end);
syncSig_sample = syncSig(1:down_sample:end);
[~, y1_norm2, burst_interval_sample] = coarseTO_estimator_v2(Sig_sample, syncSig_sample, new_sps); % Find the coarse_pilot_position, m, of the pilot
%[~, y1_norm2, burst_interval_sample] = coarseTO_estimator_v2_with_overlapadd(Sig_sample, syncSig_sample, new_sps); % Find the coarse_pilot_position, m, of the pilot

burst_interval = burst_interval_sample .* down_sample;
tElapsed6 = toc(tStart6);
% fprintf("Course TO estimation is done in %f(sec).\n", tElapsed6); % coarse_pilot_position = coarse_pilot_position(9); % First test with burst9: The position of highest peak(r[m_9])  
% figure
% plot(y1_norm2); % stem(MF_amplitude);

number_of_bursts = length(burst_interval);
start_burst = burst_interval(1,:); % coarse_pilot_position
end_burst = burst_interval(2,:);
%% Set buffers
l_hat_d_buffer = {};
b_i_hat_d_buffer = {};
b_q_hat_d_buffer = {};

time_recording_buffer = zeros(5, number_of_bursts);
% 1st row: record the time of Get process data in each burst
% 2nd row: record the time of CFO, PHO, TO estimation in each burst
% 3rd row: record the time of Reget databurst in each burst
% 4th row: record the time of CFO, PHO compensation in each burst
% 5th row: record the time of OQPSK aftendRx process in each burst
tStartIteration = tic;
for i = 1 : number_of_bursts
    %% Get processed data for CFO, PHO, TO estimation
    tStart = tic;
    K = 2000;
    v = Sig(start_burst(i) - K : start_burst(i) + K);
    tElapsed = toc(tStart);
    %fprintf("Get processed data in %f(sec).\n", tElapsed7);  
    time_recording_buffer(1,i) = tElapsed;  
    % After finding the course position of the pilot, the number of samples of ...
    % processed data in each burst for the next stage is 2*K + 1. 
    %% CFO, PHO, TO estimation
    tStart = tic;
    % [Version 1]: original version
    % [cfo_hat, pho_hat, d, energy_buffer, vCFO_buffer] = CFOPHOTO_estimator(v, syncSig, K, Ts_USRP);
    % [Version 2]: speed up version[reduce the needed loop and add discriminant to break the loop]
    % [cfo_hat, pho_hat, d, energy_buffer, vCFO_buffer] = CFOPHOTO_estimator_v2(v, syncSig, K, Ts_USRP);
    % [Version 3]: speed up version[only do one loop for each burst]
    [cfo_hat, pho_hat, d, energy_buffer, vCFO_buffer] = CFOPHOTO_estimator_v3(v, syncSig, K, Ts_USRP);    
    tElapsed = toc(tStart);
    %fprintf("CFO, PHO, TO estimation is done in %f(sec).\n", tElapsed);  
    time_recording_buffer(2,i) = tElapsed;
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
    tStart = tic;
    exact_pilot_position = start_burst(i) + d; 
    %% Reget the correct received pilot and data 
    % MODE 1: let analysis symbol in each burst be different[correspond to the given Ankang OQPSK signal]
    %[analysis_symbols, RegetReceived_pilot, Regetdata_burst] = get_databurst_v2(start_burst(i), end_burst(i), Sig, syncSig, exact_pilot_position, new_sps);       
    % MODE 2: let analysis symbol in each burst be the same[simplified version]
    tElapsed = toc(tStart);  
    end_burst = start_burst(i) + K;
    [analysis_symbols, RegetReceived_pilot, Regetdata_burst] = get_databurst_v2(start_burst(i), end_burst, Sig, syncSig, exact_pilot_position, new_sps);    
    %fprintf("Reget databurst is done in %f(sec).\n", tElapsed);  
    time_recording_buffer(3,i) = tElapsed; 
    %%  CFO&CPO compensation
    tStart = tic;
    %Received_pilot_compen =  CFOPHOcompen(RegetReceived_pilot, pho_hat, cfo_hat, 1/new_fs); 
    data_burst_compen =  CFOPHOcompen(Regetdata_burst, pho_hat, cfo_hat, 1/new_fs);
    tElapsed = toc(tStart);
    %fprintf("CFO, PHO compensation is done in %f(sec).\n", tElapsed8);  
    time_recording_buffer(4,i) = tElapsed;
    %% OQPSK aftend Rx
    tStart = tic;
    %[u_hat_p, l_hat_p, b_i_hat_p, b_q_hat_p, RxSym_p] = OQPSK_aftendRx(Received_pilot_compen, Pulseshaping, FilterSpanInSymbols, new_sps, IQinterlace);
    [u_hat_d, l_hat_d, b_i_hat_d, b_q_hat_d, RxSym_d] = OQPSK_aftendRx(data_burst_compen, Pulseshaping, FilterSpanInSymbols, new_sps, IQinterlace);
    %% Buffer
    % for pilot
    %l_hat_p_buffer(i,:) = l_hat_p;
    %b_i_hat_p_buffer(i,:) =  b_i_hat_p;
    %b_q_hat_p_buffer(i,:) =  b_q_hat_p;
    
    % for data
    L_b_hat_d = length(b_i_hat_d);
    l_hat_d_buffer{i} = l_hat_d;
    b_i_hat_d_buffer{i} =  b_i_hat_d;
    b_q_hat_d_buffer{i} =  b_q_hat_d;
    
    tElapsed = toc(tStart);
    %fprintf("OQPSK aftendRx process is done in %f(sec).\n", tElapsed9);  
    time_recording_buffer(5,i) = tElapsed;
end
tElapsedIteration = toc(tStartIteration);
%fprintf("CFO, PHO, TO processing of all bursts is done in %f(sec).\n", tElapsedIteration);  
%% vectorize data from buffer
% L = size(l_hat_p_buffer, 1) * size(l_hat_p_buffer, 2);
% vec_l_hat_p = vec(l_hat_p_buffer);       % or vec_l_hat_p = reshape(l_hat_p_buffer,L,1); % a col vector 
% vec_b_i_hat_p = vec(b_i_hat_p_buffer.'); % a row vector
% vec_b_q_hat_p = vec(b_q_hat_p_buffer.'); % a row vector
%% vectorize data from cell
vec_l_hat_d = vectorizeCell(l_hat_d_buffer);
vec_b_i_hat_d = vectorizeCell(b_i_hat_d_buffer); % a row vector
vec_b_q_hat_d = vectorizeCell(b_q_hat_d_buffer); % a row vector
%% DecisionProcess => get b_hat
tStart10 = tic;
%[b_hat_p] = DecisionProcess(vec_b_i_hat_p, vec_b_q_hat_p);
[b_hat] = DecisionProcess(vec_b_i_hat_d, vec_b_q_hat_d);
tElapsed10 = toc(tStart10);
%fprintf("Decision process is done in %f(sec).\n", tElapsed10);  
%% Reorganize bit order => get m_hat
tStart11 = tic;
%[m_hat_p] = bitReorganization(bitorder, b_hat_p); 
[m_hat] = bitReorganization(bitorder, b_hat);
tElapsed11 = toc(tStart11);
%fprintf("Bit reorganization is done in %f(sec).\n", tElapsed11);  
%% Symbol demapping
tStart12 = tic;
%[ReceivedSym_p] = symbol_demapping(m_hat_p);
[ReceivedSym] = symbol_demapping(m_hat);
tElapsed12 = toc(tStart12);
totalElapsed_end = toc(totalElapsed_start);
%fprintf("Symbol demapping is done in %f(sec).\n", tElapsed12);  
%fprintf("All process is done in %f(sec).\n", totalElapsed_end);  
%% get received pilot from received symbol
% when MODE 2 is used, i.e. K is set
vec_b_i_hat_p_K = [];
vec_b_q_hat_p_K = [];
ReceivedSym_p_K = [];
for i=1:number_of_bursts
    vec_b_i_hat_p_K = [vec_b_i_hat_p_K vec_b_i_hat_d(L_b_hat_d*(i-1)+1: L_b_hat_d*(i-1) + L_pilotPattern)];
    vec_b_q_hat_p_K = [vec_b_q_hat_p_K vec_b_q_hat_d(L_b_hat_d*(i-1)+1: L_b_hat_d*(i-1) + L_pilotPattern)];
    ReceivedSym_p_K = [ReceivedSym_p_K ReceivedSym(L_b_hat_d*(i-1)+1: L_b_hat_d*(i-1) + L_pilotPattern)];
end
%% SNR estimation
AllpilotSym = vec_b_i_hat_p_K + j * vec_b_q_hat_p_K; % when K is set
[SNR_dB] = SNRestimation("OQPSK", AllpilotSym);
%% Error rate calculation
pilotPattern_vec = [2 1 3 0 3 1 3 3 0 1 0 3 3 1 2 0 1 0 0 1 1 3 2 3 3 1 0];
[SER, BER, error_pilot] = ErrorRateCalculation("OQPSK", pilotPattern_vec, ReceivedSym_p_K);
%% Theoratical SNR/BER curve
indexSNRdB = [1:16];
[SER_theory, BER_theory] = SERBER_TheoreticalCurve("OQPSK", indexSNRdB);
%% Save file
%name = ['OQPSK_1361083_1.txt'] ;
%writematrix(ReceivedSym,name);
%% Time Recording
fprintf("********************************************************************\n");  
fprintf("************************** Time Recording **************************\n");  
fprintf("Data conversion is done in %f(sec).\n", tElapsed1);  
fprintf("DC offset of data is removed in %f(sec).\n", tElapsed2);  
fprintf("Down conversion is done in %f(sec).\n", tElapsed3);  
fprintf("Data resample is done in %f(sec).\n", tElapsed4);  
fprintf("OQPSK frontendTx process is done in %f(sec).\n", tElapsed5);  
fprintf("Course TO estimation is done in %f(sec).\n", tElapsed6);  
fprintf("CFO, PHO, TO processing of all bursts is done in %f(sec).\n", tElapsedIteration);  
fprintf("Decision process is done in %f(sec).\n", tElapsed10);  
fprintf("Bit reorganization is done in %f(sec).\n", tElapsed11);  
fprintf("Symbol demapping is done in %f(sec).\n", tElapsed12);  
fprintf("All process is done in %f(sec).\n", totalElapsed_end);  

%% printf the time-consuming of each iteration 
%{
fprintf("********************************************************************\n"); 
fprintf("CFO, PHO, TO processing of all bursts is done in %f(sec).\n", tElapsedIteration);  
fprintf("************************** CFO, PHO, TO processing of each burst **************************\n");  
for(i = 1:number_of_bursts)
    fprintf("Burst %d\n", i);  
    fprintf("Get process data in %f(sec)\n", time_recording_buffer(1,i));  
    fprintf("CFO, PHO, TO estimation is done in %f(sec)\n", time_recording_buffer(2,i));  
    fprintf("Reget databurst in %f(sec)\n", time_recording_buffer(3,i));  
    fprintf("CFO, PHO compensation is done in %f(sec)\n", time_recording_buffer(4,i));  
    fprintf("OQPSK aftendRx process is done in %f(sec)\n", time_recording_buffer(5,i));  
end
%}
%{
All_Iteration_time = 0;
for(i = 1:number_of_bursts)
    All_Iteration_time = All_Iteration_time + time_recording_buffer(1,i) + time_recording_buffer(2,i) + time_recording_buffer(3,i) + time_recording_buffer(4,i) + + time_recording_buffer(5,i);  
end
fprintf("Check for all iteration is done in %f(sec).\n", All_Iteration_time);  
%}
ReceivedSym;
%% Plot =============================================================
%% Plot constellation
plot_constellation(vec_b_i_hat_p_K, vec_b_q_hat_p_K); % Plot received pilot
plot_constellation(vec_b_i_hat_d, vec_b_q_hat_d); % Plot received pilot + received data
%% Plot Theoratical SNR v.s. SER curve and the SNR point
plotSNR_SER_BER(indexSNRdB, SNR_dB, BER, BER_theory, SER, SER_theory);
