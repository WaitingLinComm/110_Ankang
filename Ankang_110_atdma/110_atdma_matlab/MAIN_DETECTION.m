clc,clear all
%% Initialiaztion  fs 25m sym r1600k
clc,clear,close all
%% Read File
file_name = 'burst1_sr25.0_dtCplx_st8t'; %110 aTDMA
%%  Data Paremeters
fs = 25*10^6; %sampling rate of aTDMA
Ts = 1/fs;% sampling period
SR = 1.6*10^6; %symbol rate of aTDMA 
Tsy = 1/SR;%symbol period
IPOINT=ceil(fs/SR);
%% Read file
[fid,message]=fopen(file_name,'r');
Data_from_file =fread(fid , 1e6, 'uint8');%read data 
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
 f_c_subcarrier =-5459.6*10^3;%3040.1 %-2960.5 
% %carrier2 = real(carrier1).*cos(-2*pi*Ts*f_c_subcarrier*(1:length(carrier1)))-1i*imag(carrier1).*sin(-2*pi*Ts*f_c_subcarrier*(1:length(carrier1))) ;
rr= carrier1.*exp(-1i*2*pi*f_c_subcarrier*(1:length(carrier1))*Ts);
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
%% pilot
[ pilot , adaptivef , inpilot2 ] = pilot_detection( rsmooth , "None" , IPOINT , 0);

MF=abs(fftconv(rsmooth,conj(inpilot2(end:-1:1))));
MF2=fftconv(abs(rsmooth),adaptivef(end:-1:1));
pilot_tem = find_high_pilot( MF , IPOINT);

pilot_high= pilot_tem(1,:);

axis = 1 : length(MF) ;
figure(4)
plot(axis , MF )
position = find_high_pilot( MF , IPOINT);
initialposition=position-length(inpilot2)+1; %Link:ADSP handout p.146~147
diff=initialposition(2:end)-initialposition(1:end-1);
initialposition = initialposition(3:end-3);
% plot before CFO compensation
figure(10)
scatter(real(rsmooth),imag(rsmooth),'.') 
%% compensation cfo (use pilot)
numberofburst = length(initialposition)-1;
% cfo_start=-100;cfo_end=100;
clear cfo_shift_all
cfo_shift_all=[];
% sumvalue=zeros(1,201);
numberofanalysissymbol=round((length(inpilot2)+10)/IPOINT);% number of symbols contain in each burst
signal=zeros(numberofburst,numberofanalysissymbol); % burst index and info contained in the burst
nn=zeros(1,numberofburst);
mean(abs(rsmooth));
for index_whichburst = 1:numberofburst
    clear data;clear data2;clear data3;
    initial_pos=initialposition(index_whichburst);
    data=rsmooth(initial_pos:IPOINT:initial_pos+numberofanalysissymbol*IPOINT-1);% only consider the first symbol out of IPOINT data
    t=((initial_pos:IPOINT:initial_pos+numberofanalysissymbol*IPOINT-1)-1)/nfs;
    ind=1:30:length(data);
    datap = pilot ;
    for i=1:length(ind)%2
        datap(2*(i-1)+1:2*(i-1)+2)=data(ind(i):ind(i)+1); % extract pilots from received data
        tp(2*(i-1)+1:2*(i-1)+2)=t(ind(i):ind(i)+1); % the time index of pilots
    end    
%     for i=1:length(ind)%3
%         datap(3*(i-1)+1:3*(i-1)+3)=data(ind(i):ind(i)+2); % extract pilots from received data
%         tp(3*(i-1)+1:3*(i-1)+3)=t(ind(i):ind(i)+2); % the time index of pilots
%     end
%     for i=1:length(ind)%4
%         datap(4*(i-1)+1:4*(i-1)+4)=data(ind(i):ind(i)+3); % extract pilots from received data
%         tp(4*(i-1)+1:4*(i-1)+4)=t(ind(i):ind(i)+3); % the time index of pilots
%     end    
    
    
    cfodata=datap./(pilot); 

%plot objective function of the first burst
    ff_ = [-1000:0.1:1000];
    objetive_ = [];
    for ff  = ff_
        objetive = var((cfodata.*exp(-1j*2*pi*ff*tp)));
        objetive_ = [objetive_ objetive];     
    end
%     figure;
%     plot(ff_,objetive_);
%     %plot([1:length(objetive_)],objetive_);
%     xlabel("CFO(Hz)");
%     ylabel("magnitude");
    
    [min_objetive,index] = min(objetive_);
    if length(index) ~= 1
        index= index(1);
    end
    cfo = ff_(index);
    
    
    cfo_shift_all=[cfo_shift_all cfo];
    
    
    data2=data.*exp(-1j*2*pi*cfo*t);
    %% compensation carrier phase offset(cpo) (use pilot)
    for i=1:length(ind)%2
        datap(2*(i-1)+1:2*(i-1)+2)=data2(ind(i):ind(i)+1); % extract pilots from received data
    end
%     for i=1:length(ind)%3
%         datap(3*(i-1)+1:3*(i-1)+3)=data2(ind(i):ind(i)+2); % extract pilots from received data
%     end
%     for i=1:length(ind)%4
%         datap(4*(i-1)+1:4*(i-1)+4)=data2(ind(i):ind(i)+3); % extract pilots from received data
%     end    
%     figure;
%     scatter(real(datap./(pilot)) , imag(datap./(pilot)))
    ph1 = angle(sum(datap./(pilot)));    
    ph2 = sum(angle(datap./(pilot)*exp(-j*ph1)))/length(pilot);
    ph = ph1+ph2 ;
    
    datap1 =datap *exp(-j*ph) ;
  
    
    data3=data2*exp(-j*ph);
    signal(index_whichburst,:)=data3;
    
%     figure;
%     scatter(real(datap1),imag(datap1))
%     figure;
%     scatter(real(pilot),imag(pilot))  
%     figure;
%     scatter(real(data3),imag(data3))
end

signal_real = real(reshape(signal,[],1)) ;
signal_imag = imag(reshape(signal,[],1)) ;
figure;
scatter(signal_real,signal_imag,'.') % Show it by using MATLAB command window
%% Collect all of the pilots in each burst as a vector
datapilot_register = []
for pilot_index_tar = 1 : numberofburst
    data_pilot = rsmooth(initialposition(pilot_index_tar): IPOINT : initialposition(pilot_index_tar) + 27*IPOINT-1);
    datapilot_register = [datapilot_register data_pilot];
end    
%% Calculate the Bit Error Rate of the signal through the received pilots & Plot the Calculated Bit Error Rate and the Theratical Bit Error Rate curve
[BER , SER] = BER_calculation("QPSK", pilot , datapilot_register);