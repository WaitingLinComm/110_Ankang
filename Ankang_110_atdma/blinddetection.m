function [timeing_offsetall,match,compen_data,compen_data2]=blinddetection(data ,sps ,span ,size_symbol ,fs ,cfo_start, cfo_end,pulse)
l_pulse=span*sps;
latency=30*sps;
sample_start=1;
sample_end=length(data) -11000  ;
sample_interval=sps*size_symbol;
compen_data=[];
compen_data2=[];
match=[];
timeing_offsetall=[];
for index=sample_start:sample_interval:sample_end
    if index==sample_start
        new_data=data(index:index+(sample_interval-1)+latency);
        t=0:1/fs:(sample_interval-1+latency)/fs;
        variance=zeros(1,length(cfo_start:cfo_end));
        timing=zeros(1,length(cfo_start:cfo_end));
        for ee=cfo_start:cfo_end
            signal_comp=new_data.*exp(j*2*pi*ee*t);
            signal_comp_afpulse = conv(signal_comp, pulse);
            signal_comp_afpulse = signal_comp_afpulse(l_pulse+1:end-l_pulse);
            num=floor(length(signal_comp_afpulse)/sps);        % number of receiver bits=1324 / floor: 狾ㄧ计 ┕璽礚絘よ俱计
            tmp=reshape(signal_comp_afpulse(1:sps*num),sps,num);   % tmp=[4*1324]
            tmp1=sum(abs(tmp).').^2;                    % 
            [val,idx]=max(tmp1);                        % т程竚
            timing(ee-cfo_start+1)=idx;
            I=signal_comp_afpulse(1,idx:sps:end);
            p=(I/mean(abs(I))).^4;
            variance(ee-cfo_start+1)=var(p);
        end
        [vars,cfo_index]=min(variance);
        cfo=cfo_index+cfo_start-1;
        timeing_offset=timing(cfo_index);
    else
        t=0:1/fs:(sample_interval-1+latency)/fs;
        t=t+(index-1)/fs;
        new_data=data(index:index+sample_interval-1+latency);
        variance=zeros(1,201);
        timing2=zeros(1,201);
        for ee=-100:100
            signal_comp=new_data.*exp(j*2*pi*(cfo+ee)*t);
            signal_comp_afpulse = conv(signal_comp, pulse);
            signal_comp_afpulse = signal_comp_afpulse(l_pulse+1:end-l_pulse);
            num=floor(length(signal_comp_afpulse)/sps);        % number of receiver bits=1324 / floor: 狾ㄧ计 ┕璽礚絘よ俱计
            tmp=reshape(signal_comp_afpulse(1:sps*num),sps,num);   % tmp=[4*1324]
            tmp1=sum(abs(tmp).').^2;                    % 
            [val,idx]=max(tmp1);                        % т程竚
            timing2(ee+100+1)=idx;
            I=signal_comp_afpulse(1,idx:sps:end);
            p=(I/mean(abs(I))).^4;
            variance(ee+100+1)=var(p);
        end
        [vars,cfo_index]=min(variance);
        cfo_shift=cfo_index-101;
        timeing_offset=timing2(cfo_index);
    end
   %% compensation 
    if index ==sample_start
        signal_comp=new_data.*exp(j*2*pi*cfo*t);
    else
        signal_comp=new_data.*exp(j*2*pi*(cfo+cfo_shift)*t);
    end
    signal_comp_afpulse = conv(signal_comp, pulse);
    signal_comp_afpulse = signal_comp_afpulse(l_pulse+1:end-l_pulse);
    I=signal_comp_afpulse(1,timeing_offset:sps:end);
    p=(I/mean(abs(I))).^4;
    phase=angle(mean(p));
    if phase<0
        phase=phase+2*pi;
    end
    I=I.*exp(-j*(phase-pi)/4);
    signal_comp_afpulse=signal_comp_afpulse.*exp(-j*(phase-pi)/4);
    I2=pskdemod(I,4,pi/4);
    %% combine two section
    if index~=sample_start
        error=zeros(1,4);
        match=zeros(1,3);
        matchindex=zeros(1,3);
        for pos=-1:1
            for i=1:4
                count=0;
                err=compen_data2(1,end-28:end)-mod(I2(1,2+pos:30+pos)+i,4);
                for g=1:29
                    if err(g)==0
                        count=count+1;
                    end
                end
                error(i)=count;
            end
            [valerror,inder]=max(error);
            match(pos+2)=valerror;
            matchindex(pos+2)=inder;
        end
        [valll,idxx]=max(match);
        inderror=matchindex(idxx);
%         timeing_offset
        I=I*exp(j*0.5*pi*inderror);
        I2=mod(I2+inderror,4);
        poss=idxx-2;
        compen_data=[compen_data I(1,31+poss:end)];
        compen_data2=[compen_data2 I2(1,31+poss:end)];
    else
        compen_data=I;
        compen_data2=I2;
    end
    timeing_offsetall=[timeing_offsetall timeing_offset];
end