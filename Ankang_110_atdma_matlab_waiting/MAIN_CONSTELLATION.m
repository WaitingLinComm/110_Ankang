clc,clear all
%% Initialiaztion  
clc,clear,close all
%%  Data Paremeters
fsy = 1.6 * 10^6;           % Symbol frequency(rate) 
Tsy = 1/fsy;                % Symbol period

% USRP info.
fs_USRP = 25 * 10^6;         % Sampling frequency(rate) 
Ts_USRP = 1/fs_USRP;         % Sampling period

% Sample per Symbol(sps)
sps_USRP = fs_USRP/fsy;      % sps_USRP = fs_USRP/fsy = 15.625 %(old sps)
IPOINT = ceil(fs_USRP/fsy);  % IPOINT = ceil(fs_USRP/fsy) = 16 %(new sps)
%% Read file
file_name = 'burst1_sr25.0_dtCplx_st8t'; %110 aTDMA
[fid,message] = fopen(file_name, 'r');
%Data_from_file = fread(fid ,  1/4 * 1e6, 'uint8');%read data 1/4 *
Data_from_file = fread(fid ,  1e6, 'uint8');
fclose(fid);
%% Convert Unsigned Interger to Signed Interger
for i = 1:length(Data_from_file)
    if Data_from_file(i) > 128
        Data_from_file(i) = Data_from_file(i) - 256; % 2^8 = 256 
    end
end
%% Convert Data into Complex Form 
b = reshape(Data_from_file, 2, length(Data_from_file)/2); % seperate real part and imaginary part
data = b(1,:) + 1j * b(2,:);                              % compose real part and imaginary part
clear b ;                                                 % clear loaded data
%% Plot Spectrum
% figure(1)
% plotspec(data, Ts_USRP);
%% Down converter
f_c = -5459.6 * 10^3; % Hz
Baseband_data = data .* exp(-1i * 2 * pi * f_c * (1:length(data)) * Ts_USRP);
% figure(2)
% plotspec(Baseband_data, Ts_USRP);
%% Resample
old_sps = sps_USRP;
new_sps = IPOINT;       % IPOINT = ceil(fs_USRP/fsy) = 16 %(new sps)   
N = new_sps / old_sps;  % upsample by N. NOTE: m can be a non-integer number
cpu = 8;                % cpu core of computer(used in parfor)
window = 6;             % half-sided window length(used in SincInterpolation)  
[data_after_Resample] = resample(Baseband_data, N, cpu, window);
nfs = fs_USRP * N;      % new sampling rate(after resample)
%%  Pulse Shaping Filter Initialization
RolloffFactor = 0.3;
FilterSpanInSymbols = 6;
SRRC_pulseshaping_filter = SRRC(new_sps, RolloffFactor, FilterSpanInSymbols);  
%% Implement Pulse Shaping Filter(Matched Filter)
signal_after_SRRC = conv(data_after_Resample, SRRC_pulseshaping_filter); % length(rsmooth) = length(data_after_Resample) + length(SRRC_pulseshaping_filter) - 1 
% signal_after_SRRC = filter(SRRC_pulseshaping_filter, 1, rsmooth); % length(signal_after_SRRC) = length(rsmooth);
rsmooth = signal_after_SRRC/mean(abs(signal_after_SRRC));
%% Pilot Mapping
[pilot, adaptivef, inpilot] = pilot_map("atdma_qpsk", 3, IPOINT); % pilot_detection( rsmooth , "None" , IPOINT , 0);
%% Pilot Position Finding
% Match filtering pilot with signal
MF = abs(fftconv(rsmooth, conj(inpilot(end:-1:1))));
figure(4)
plot((1 : length(MF)), MF);
MF2 = fftconv(abs(rsmooth), adaptivef(end:-1:1));
% figure(5)
% plot((1 : length(MF2)), MF2);

% Find the peak in the MF figure
position = find_high_pilot(MF , IPOINT);

% Find the exact pilot position 
pilotpos = position - length(inpilot) + 1; % First row records start pilot position and second row records end pilot positions.
diff = pilotpos(2:end) - pilotpos(1:end-1);
pilotpos_process = pilotpos(3:end - 3); % Since we find the first two columns of pilot position is negative, so we adopt the pilot position after the thrid position.

%% Plot constellation before CFO, PHO compensation.
% figure(10)
% scatter(real(rsmooth),imag(rsmooth),'.') 
%% Compensation cfo (use pilot)
numberofburst = length(pilotpos_process);
CFO_searching_region = (-1000 : 0.1 : 1000); % CFO searching region.  CFO_searching_region = [cfo_start : 0.1 : cfo_end]; 
cfo_shift_all = [];

%numberofanalysissymbol = round((length(inpilot) + 10)/IPOINT); % ? % number of symbols contain in each burst
numberofanalysissymbol = round((length(inpilot))/IPOINT);
demod_sig = zeros(numberofburst, numberofanalysissymbol); % burst index and info contained in the burst
nn = zeros(1,numberofburst);

for index_whichburst = 1 : numberofburst - 1
    initial_pos = pilotpos_process(index_whichburst); 
    data = rsmooth(initial_pos : IPOINT : initial_pos + numberofanalysissymbol * IPOINT - 1); % Only extract the first symbol out of IPOINT data.
    t = ((initial_pos : IPOINT : initial_pos + numberofanalysissymbol * IPOINT - 1) -1)/nfs;
    
    ind = 1 : 30 : length(data);
    for i = 1 : length(ind) % 2
        datap(2 * (i - 1) + 1 : 2 * (i - 1) + 2) = data(ind(i) : ind(i) + 1); % extract pilots from received data
        tp(2 * (i - 1) + 1 : 2 * (i - 1) + 2) = t(ind(i) : ind(i) + 1); % the time index of pilots
    end    
    % for i=1:length(ind) % 3
    %     datap(3*(i-1)+1:3*(i-1)+3)=data(ind(i):ind(i)+2); % extract pilots from received data
    %     tp(3*(i-1)+1:3*(i-1)+3)=t(ind(i):ind(i)+2); % the time index of pilots
    % end
    % for i=1:length(ind) % 4
    %     datap(4*(i-1)+1:4*(i-1)+4)=data(ind(i):ind(i)+3); % extract pilots from received data
    %     tp(4*(i-1)+1:4*(i-1)+4)=t(ind(i):ind(i)+3); % the time index of pilots
    % end    
    
    
    cfodata = datap./(pilot); 

%plot objective function of the first burst

    objetive_ = [];
    for ff  = CFO_searching_region
        objetive = var((cfodata .* exp(-1j * 2 * pi * ff * tp)));
        objetive_ = [objetive_ objetive];     
    end
    % figure;
    % plot(ff_,objetive_);
    % %plot([1:length(objetive_)],objetive_);
    % xlabel("CFO(Hz)");
    % ylabel("magnitude");
    
    [min_objetive,index] = min(objetive_);
    if length(index) ~= 1
        index= index(1);
    end
    cfo = CFO_searching_region(index);
    
    
    cfo_shift_all=[cfo_shift_all cfo];
    
    data2 = data .* exp(-1j * 2 * pi * cfo * t);
    %% Compensation carrier phase offset(cpo) (use pilot)
    for i=1:length(ind)%2
        datap(2*(i-1)+1:2*(i-1)+2)=data2(ind(i):ind(i)+1); % extract pilots from received data
    end
    % for i=1:length(ind)%3
    %     datap(3*(i-1)+1:3*(i-1)+3)=data2(ind(i):ind(i)+2); % extract pilots from received data
    % end
    % for i=1:length(ind)%4
    %     datap(4*(i-1)+1:4*(i-1)+4)=data2(ind(i):ind(i)+3); % extract pilots from received data
    % end    
    % figure;
    % scatter(real(datap./(pilot)) , imag(datap./(pilot)))
    
    %% Estimate PHO   
    % sol1:
    % ph1 = angle(mean(datap./(pilot))); 
    % ph2 = sum(angle(datap./(pilot)*exp(-j * ph1))) / length(pilot);
    % ph = ph1+ph2 ;
    
    % sol1:
    pho = angle(mean(datap./(pilot)));
    datap1 =datap * exp(-1j * pho);
  
    
    data3=data2*exp(-j*pho);
    demod_sig(index_whichburst,:)=data3;
    
%     figure;
%     scatter(real(datap1),imag(datap1))
%     figure;
%     scatter(real(pilot),imag(pilot))  
%     figure;
%     scatter(real(data3),imag(data3))
end

%demod_sig_real = real(reshape(demod_sig, [], 1)) ;
demod_sig_real = real(reshape(demod_sig, [], 1)) ;
demod_sig_imag = imag(reshape(demod_sig, [], 1)) ;
figure;
scatter(demod_sig_real, demod_sig_imag, '.'); % Show it by using MATLAB command window
%% Collect all of the pilots in each burst as a vector
datapilot_register = [];
for pilot_index_tar = 1 : numberofburst
    data_pilot = rsmooth(pilotpos_process(pilot_index_tar): IPOINT : pilotpos_process(pilot_index_tar) + 27*IPOINT-1);
    datapilot_register = [datapilot_register data_pilot];
end    
%% Calculate the Bit Error Rate of the signal through the received pilots & Plot the Calculated Bit Error Rate and the Theratical Bit Error Rate curve
[BER , SER] = BER_calculation("QPSK", pilot , datapilot_register);