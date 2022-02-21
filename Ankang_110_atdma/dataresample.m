function [d_sum_all,const]=dataresample(C_shift,sps,fs,new_sps,id,cpu)  %#codegen
% n=floor(log10(sps))+1; %find ten digits.
% const=sps/(10^n); %resample new period. ex:sps=9.0419 to sps =10 ,const=0.90419;
const = 0 ;
const=sps/new_sps;
d_sum_all=(1+j)*zeros(1,fix(length(C_shift)/const)); %initial number  
d_each=(1+j)*zeros(1,fix(length(C_shift)/const));
for k2=id:cpu:fix(length(C_shift)/const)-1
    k2_match=round(const*k2);
    if (k2_match>=5 & k2_match<length(C_shift)-5 )
        for  k1=k2_match-5:k2_match+5;    %10
             k3=-k2*const+k1;
             if k3==0
                 d_each(k2+1)=C_shift(k1+1).*1;
             else
                 d_each(k2+1)=C_shift(k1+1).*sin(pi*k3)/(pi*k3); %.*sinc(2*(fs/2)*k2*(const/fs)-k1);
             end
            d_sum_all(k2+1)=d_each(k2+1)+d_sum_all(k2+1);
        end
    elseif (k2_match<5 )
        for  k1=0:k2_match+5;  %3 
            k3=-k2*const+k1;
             if k3==0
                 d_each(k2+1)=C_shift(k1+1).*1;
             else
                 d_each(k2+1)=C_shift(k1+1).*sin(pi*k3)/(pi*k3); %.*sinc(2*(fs/2)*k2*(const/fs)-k1);
             end
            d_sum_all(k2+1)=d_each(k2+1)+d_sum_all(k2+1);
        end
    else
        for  k1=k2_match:length(C_shift)-1;
            k3=-k2*const+k1;
             if k3==0
                 d_each(k2+1)=C_shift(k1+1).*1;
             else
                 d_each(k2+1)=C_shift(k1+1).*sin(pi*k3)/(pi*k3); %.*sinc(2*(fs/2)*k2*(const/fs)-k1);
             end
            d_sum_all(k2+1)=d_each(k2+1)+d_sum_all(k2+1);
        end
    end
end
end