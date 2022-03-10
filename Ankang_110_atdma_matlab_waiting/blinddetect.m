function [big_star ,big_sym_all ,cfo,cfo_shift_all ,time_offset_all ,var_all]=blinddetect (data ,sps ,span ,size_symbol ,fs ,cfo_start, cfo_end,pulse  )
cfo_all=0;
time_offset_all=0;
var_all=0;
cfo_shift_all=0;
sample_start=1;
sample_end=length(data);
sample_interval=sps*size_symbol;
l_pulse=span*sps;
latency=30*sps+l_pulse;
for index=sample_start:sample_interval:sample_end 
    if index==sample_start
        new_data=data(index:index+(sample_interval-1)+latency); %多拉第二段的前60個點 
        eee=0;
        variance=zeros(1,cfo_end-cfo_start+1);
        tt=zeros(1,cfo_end-cfo_start+1);
        t=0:1/fs:(sample_interval-1+latency)/fs;
        for ee=cfo_start:1:cfo_end
            eee=eee+1;
            % Filter and downsample the received signal for matched filtering.
            signal_comp=new_data.*exp( j*2*pi*ee*t);
            signal_comp_afpulse = conv(signal_comp, pulse); %pulse is match filter
            signal_comp_afpulse = signal_comp_afpulse(l_pulse+1:end-l_pulse);  %去掉頭尾60個sample點
            hh=0;
            I=zeros(1,length(signal_comp_afpulse)/sps);
            for i=1:sps %i=1~sps
                hh=hh+1;
                gg=0;
                kk=i:sps:i+sps*(length(signal_comp_afpulse)/sps-1);
                I=signal_comp_afpulse(kk);
                p=(I/mean(abs(I))).^4;
                varrr(hh)=var(p);
            end
            [variance(eee),tt(eee)]=min(varrr);
        end
        [vars, cfo_index]=min(variance);
        cfo=cfo_start-1+cfo_index;
        time_offset=tt(cfo_index);
        
    else
        
        if (index+sample_interval-1+latency)>length(data)
            new_data=data(index:end);
            t=0:1/fs:(length(new_data)-1)/fs;
            t=t+(index-1)/fs;
        else
            new_data=data(index:index+sample_interval-1+latency);
            t=0:1/fs:(sample_interval-1+latency)/fs;
            t=t+(index-1)/fs;
        end
        eee=0;
        variance=zeros(1,101);
        tt=zeros(1,101);
        for ee=-50:1:50
            eee=eee+1;
            % Filter and downsample the received signal for matched filtering.
            signal_comp=new_data.*exp(j*2*pi*(cfo+ee)*t);
            signal_comp_afpulse = conv(signal_comp, pulse);
            signal_comp_afpulse = signal_comp_afpulse(l_pulse+1:end-l_pulse);  %去掉頭尾60個sample點
            hh=0;
            I=zeros(1,floor(length(signal_comp_afpulse)/sps));
            for i=1:sps %i=1~sps
                hh=hh+1;
                gg=0;
                kk=i:sps:i+sps*(length(signal_comp_afpulse)/sps-1);
                I=signal_comp_afpulse(kk);
                p=(I/mean(abs(I))).^4;
                varrr(hh)=var(p);
            end
            [variance(eee),tt(eee)]=min(varrr);
        end
        [vars, cfo_index]=min(variance);
        cfo_shift=-50-1+cfo_index;
        time_offset=tt(cfo_index);
    end
    %% first compensation
    % Filter and downsample the received signal for matched filtering.
    if index ==sample_start
        signal_comp=new_data.*exp(j*2*pi*cfo*t);
    else
        signal_comp=new_data.*exp(j*2*pi*(cfo+cfo_shift)*t);
    end
    signal_comp_afpulse = conv(signal_comp, pulse);
    signal_comp_afpulse = signal_comp_afpulse(l_pulse+1:end-l_pulse);  %去掉頭尾60個sample點
    for i=time_offset %i=1~sps
        gg=0;
        kk=-sps+i;
        while   kk<=length(signal_comp_afpulse)-sps  %小心會爆掉
            gg=gg+1;
            kk=kk+sps;
            I(gg)=signal_comp_afpulse(kk);
        end
        p=(I/mean(abs(I))).^4; %% for phase compensation
        angleshift=angle(mean(p));
        if angleshift<0
            angleshift=angleshift+2*pi;
        end
    end
    p=(I/mean(abs(I))).^4;
    I=I.*exp(-j*(angleshift-pi)/4);      %把相位拉到45度
    % I=I.*exp(j*pi);                    % already know that I need to turn 180 degree
    big_sym=pskdemod(I,4,pi/4);
    %% combine two section
    if index~=sample_start
        max_cout=0;
        for angle_shift=1:4
            for pos_shift=-1:1
                big_sym_turn=mod(big_sym+angle_shift-1,4);
                big_overlap=big_sym_turn(10:20)-big_sym_all(end-20+pos_shift:end-10+pos_shift);
                big_overlap_cout=0;
                for lap=1:length(big_overlap)
                    if big_overlap(lap)==0
                        big_overlap_cout=big_overlap_cout+1;
                    end
                end
                if big_overlap_cout>max_cout
                    max_cout=big_overlap_cout;
                    big_overlap_pos=pos_shift;
                    big_overlap_turn=angle_shift;
                    bigsymturn=big_sym_turn;
                    if max_cout == 11
                        break;
                    end
                end
            end
        end
        big_sym_all = [big_sym_all(1:end-21+big_overlap_pos) bigsymturn(10:end)];
        big_star = [big_star(1:end-21+big_overlap_pos) I(10:end).*exp(j*(big_overlap_turn-1)*pi/2)];
%                 if  time_offset-time_offset_all(end)<0
%                     %             big_sym(1:31)-big_sym_all(end-30:end)
%                     if mode(big_sym(1:31)-big_sym_all(end-30:end)) ~=0
%                         k_shift=mode(mod((big_sym(1:31)-big_sym_all(end-30:end)),4));
%                         big_sym=mod(big_sym-k_shift,4);
%                         I=I.*exp(-k_shift*j*pi/2);
%                     end
%                     big_sym(1:10)-big_sym_all(end-30:end-30+9);
%                     big_sym_all = [big_sym_all(1:end-31) big_sym];
%                     big_star = [big_star(1:end-31) I];
%                 else
%                     %             big_sym(1:30)-big_sym_all(end-29:end)
%                     if mode(big_sym(1:30)-big_sym_all(end-29:end)) ~=0
%                         k_shift=mode(mod((big_sym(1:30)-big_sym_all(end-29:end)),4));
%                         big_sym=mod(big_sym-k_shift,4);
%                         I=I.*exp(-k_shift*j*pi/2);
%                     end
%                     big_sym(1:10)-big_sym_all(end-29:end-29+9);
%                     big_sym_all=[big_sym_all(1:end-30) big_sym];
%                     big_star = [big_star(1:end-30) I];
%                 end
    else
        big_sym_all=big_sym;
        big_star=I;
    end
%     if index == sample_start
%         big_sym_all = big_sym_all(2:end);
%         big_star = big_star(2:end);
%     end
    cfo_all=[cfo_all cfo];
    time_offset_all=[time_offset_all time_offset];
    var_all=[var_all vars];
    if index~=sample_start
        cfo_shift_all=[cfo_shift_all cfo_shift];
    end
end
%big symbol information
cfo_all=cfo_all(2:end);
time_offset_all=time_offset_all(2:end);
var_all=var_all(2:end);
% cfo_shift_all=cfo_shift_all(2:end);