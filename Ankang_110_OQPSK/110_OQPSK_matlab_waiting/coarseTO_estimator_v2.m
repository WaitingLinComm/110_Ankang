function [y1, y1_norm2, m] = coarseTO_estimator_v2(r, q, sps)
%{
% coarseTO_estimator_v2
% 
% [Input variable ]: r(a vector), q(a vector), sps(a scalar)
% [Output variable]: y1(a vector), y1_norm2(a vector), m(a vector) 
% 
% sps:  SamplesPerSymbol
% m:    course pilot position index of r
% 
% call function: find_burst_interval.m
%}
% Match filtering r with q
y1 = filter(conj(q(end : -1 : 1)), 1, r); % or y1 = fftconv(conj(q(end : -1 : 1)), r);
y1_norm2 = abs(y1) .^ 2; % or y1_energy = abs(y1) .^ 3;
[max_y1, max_y1_index] = max(y1_norm2); 
%figure
%plot(y1_norm2)
% sol1:
%n  = find_high_pilot( y1_norm2 , sps); % the course pilot position index of y1, which the index corresponding to the local maximum of y1_norm2
% sol2:
burst_interval = find_burst_interval( y1_norm2 , sps);  
% 1st row: head position of the burst(i.e. the pilot position)
% 2nd row: end position of the burst
m  = burst_interval - length(q) + 1;    % the course pilot position index of r
%% Calculate avg_power of pilot peaks

start_position = burst_interval(1,:);
[r, number_of_pilot] = size(start_position);
% y1_abs_pilot = abs(y1(start_position));
% avg_power = sum(sqrt(y1_abs_pilot))/number_of_pilot;

y1_abs_pilot = sqrt(y1_norm2(start_position));
avg_power = sum(sqrt(y1_abs_pilot))/number_of_pilot;

% normalized_y1_pilot = y1_pilot/norm(y1_pilot,2); % with normalized
% avg_power = sum(abs(normalized_y1_pilot)); 
end
