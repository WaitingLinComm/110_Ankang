function y = extract_pilot_signal( IPOINT , N_symbol_burst ,  N_pilot_burst , pilot_signal )
y = zeros(1, length(pilot_signal)) ;
for i = 1 : length( pilot_signal )/IPOINT /N_symbol_burst 
    for j = 1 : N_pilot_burst
        y( (i-1)*N_symbol_burst*IPOINT + (j-1)*IPOINT + 1 ) =  1   ;
    end
end
y = y.*pilot_signal ;
end