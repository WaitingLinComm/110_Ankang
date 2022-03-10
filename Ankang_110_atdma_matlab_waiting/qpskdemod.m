function [demodata]=qpskdemod(data)
demodata=zeros(1,length(data));
for i=1:length(data)
    if real(data(i))>0
        demodata(i)=1;
    else
        demodata(i)=-1;
    end
    
    if imag(data(i))>0
        demodata(i)=demodata(i)+j;
    else
        demodata(i)=demodata(i)-j;
    end
end
