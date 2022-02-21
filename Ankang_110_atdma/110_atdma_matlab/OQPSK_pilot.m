function y = OQPSK_pilot( mode_index )
%% parameter setting 
% output y is the pilot symbol sequence stored in an array
% mode_index ranges from 1 to 5 according to known_burst_list_OQPSK.txt
%% initialize the pilot sequence using mode_index
% mode_index is based on known_burst_list_OQPSK.txt
p111 = '' ;
if mode_index == 1
    p111 = '203032102200023132132211222' ;
elseif mode_index == 2
    p111 = '021211331131101310000232201112322102300230121203' ;
elseif mode_index == 3
    p111 = '231013110301132030033121130' ;            
elseif mode_index == 4
    p111 = '203320323203011130322301302' ;
elseif mode_index == 5
    p111 = '213031330103312010011323310';
else
    disp("invalid mode_index in OQPSK")
end    

pilot=zeros(1 , length(p111));
for i=1:2:length(p111)
    if (p111(i)=='0')
        %pilot=[pilot 1+1j];
        pilot(i) = 1+1j;
    elseif (p111(i)=='1')
        %pilot=[pilot -1+1j];
        pilot(i) = -1+1j;
    elseif (p111(i)=='2')
        %pilot=[pilot -1-1j];
        pilot(i) = -1-1j;
    elseif (p111(i)=='3')
        %pilot=[pilot 1-1j];
        pilot(i) = 1-1j;
    end
end
for i=2:2:length(p111)
    if (p111(i)=='0')
        %pilot=[pilot 1j];
        pilot(i) = 1j;
    elseif (p111(i)=='1')
        %pilot=[pilot -1];
        pilot(i) = -1;
    elseif (p111(i)=='2')
        %pilot=[pilot -1j];
        pilot(i) = -1j;
    elseif (p111(i)=='3')
        %pilot=[pilot 1];
        pilot(i) = 1;
    end
end

y = pilot ;
end
