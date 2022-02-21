function [y,threshold] = detect_success( signal , inpilot2 , IPOINT)

MF1=abs(fftconv(signal,conj(inpilot2(length(inpilot2):-1:1))));

position = find_high_pilot( MF1 , IPOINT); 


threshold = 0 ;

if length(position) < 5 % less than 5 bursts
    y = 0 ;
elseif length(position) >= 5 % more than 5 bursts
    y = 1 ;
    running_mean = 0 ;
    for i = 1 : 20
        for j  = 1: 5
            running_mean = running_mean + MF1(  position(j) + i * IPOINT )/100 ;
        end
    end
    threshold = mean(MF1(position(1:5))) / running_mean ;
end

end