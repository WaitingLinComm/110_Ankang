function y = extract_pilot_sequence( IPOINT , N_symbol_burst ,  N_pilot_burst , pilot_signal )
y = zeros(1, length( pilot_signal )/IPOINT /N_symbol_burst *  N_pilot_burst) ;
for i = 1 : length( pilot_signal )/IPOINT /N_symbol_burst 
    for j = 1 : N_pilot_burst
        y( (i-1)*N_pilot_burst + (j-1) + 1 )  = pilot_signal( (i-1)*N_symbol_burst*IPOINT + (j-1)*IPOINT + 1 )    ;
    end
end
end