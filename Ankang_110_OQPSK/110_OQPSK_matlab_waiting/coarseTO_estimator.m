function [y1, y1_norm2, m] = coarseTO_estimator(r, q, sps)
% coarseTO_estimator
% 
% [Input variable ]: r(a vector), q(a vector), sps(a scalar)
% [Output variable]: y1(a vector), y1_norm2(a vector), m(a vector) 
% 
% sps:  SamplesPerSymbol
% m:    course pilot position index of r
% 
% call function: find_high_pilot.m

% Match filtering r with q
flip_q = q(end : -1 : 1);
y1 = filter(conj(flip_q), 1, r); % or y1 = fftconv(conj(q(end : -1 : 1)), r);
y1_norm2 = abs(y1) .^ 2; % or y1_energy = abs(y1) .^ 3;
[max_y1, max_y1_index] = max(y1_norm2); 
%figure
%plot(y1_norm2);
n  = find_high_pilot( y1_norm2 , sps);  % the course pilot position index of y1, which is the index corresponding to the local maximum of y1_norm2
m  = n - length(q) + 1;                 % the course pilot position index of r
end