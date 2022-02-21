clc,clear all
%% Initialiaztion  fs 25m sym r1600k
clc,clear,close all
%% Read File
file_name = 'burst1_sr25.0_dtCplx_st8t'; %110 aTDMA
%%  Data Paremeters
fs = 25*10^6; %sampling rate of aTDMA
Ts = 1/fs;% sampling period
SR = 2.4*10^6; %symbol rate of aTDMA 
Tsy = 1/SR;%symbol period
IPOINT=11;
%% Read file
[fid,message]=fopen(file_name,'r');
Data_from_file =fread(fid,5e5, 'uint8');%read data 
fclose(fid);

%% Convert Unsigned Interger to Signed Interger Using Spec
for i=1:length(Data_from_file)
    if Data_from_file(i)>128
        Data_from_file(i)=Data_from_file(i)-256; %2^8 = 256 
    end
end

%% Convert Data into Complex Form 
b_=reshape(Data_from_file,2,length(Data_from_file)/2); % seperate real part and imaginary part
data=b_(1,:)+1j*b_(2,:); % compose real part and imaginary part
clear b_ ;%clear Data_from_file; % clear temporary data

%% Plot Spectrum of OQPSK data 
% figure(1)
% plotspec(data,Ts);

carrier1 = data;

clear data;


%% down converter
 f_c_subcarrier = 2954.1*10^3; 
% %carrier2 = real(carrier1).*cos(-2*pi*Ts*f_c_subcarrier*(1:length(carrier1)))-1i*imag(carrier1).*sin(-2*pi*Ts*f_c_subcarrier*(1:length(carrier1))) ;
rr= carrier1.*exp(-1i*2*pi*Ts*f_c_subcarrier*(1:length(carrier1)));
clear carrier1;
% 
figure(2)
plotspec(rr,Ts);
%%  Resample
intfac=IPOINT/(Tsy/Ts);
cpu=4;
d_sum_all=zeros(cpu,fix(length(rr)*intfac));
parfor id=0:cpu-1
    [d_sum_all(id+1,:),const]=dataresample(rr,Tsy/Ts,1/Ts,IPOINT,id,cpu);
end
rsmooth=sum(d_sum_all);
nfs=intfac * fs;
%%  Shaping Filter initialization
L=6;   
rolloff=0.3;                          % SRRC filter rolloff factor 
rcpul=rcosine(1,IPOINT,'sqrt',rolloff,L);  % SRRC pulse shaping
%% convolution shaping filter(match filter)
rsmooth=conv(rsmooth,rcpul);

rsmooth=rsmooth/mean(abs(rsmooth));

N_symbol_burst = 30;
pilot_len = 60;
N_pilot_burst = 2;
pilot_signal_length = pilot_len / N_pilot_burst * N_symbol_burst * IPOINT ;
num_symbol_burst = N_symbol_burst * pilot_len / N_pilot_burst ;
pilot_signal = zeros( 1 , pilot_signal_length ) ;

objective = 1 : length(rsmooth) - pilot_signal_length ;
for i = 1 : length(rsmooth) - pilot_signal_length 
    pilot_signal_test = extract_pilot_signal( IPOINT , N_symbol_burst ,  N_pilot_burst , rsmooth(i:i+pilot_signal_length-1) ) ;
    objective(i) = mean(  maxk(  fftconv(  rsmooth , conj(pilot_signal_test(end:-1:1) )   ) , 50  )  );
    
    
    
end
[max_v , max_i] = max(objective);
pilot_signal_test = extract_pilot_signal( IPOINT , N_symbol_burst ,  N_pilot_burst , rsmooth(max_i:max_i+pilot_signal_length-1) ) ;
figure;
MF =  fftconv(  rsmooth , conj(pilot_signal_test(end:-1:1) )   ) ;
axis = 1 :length(MF) ;
plot(axis,MF)


