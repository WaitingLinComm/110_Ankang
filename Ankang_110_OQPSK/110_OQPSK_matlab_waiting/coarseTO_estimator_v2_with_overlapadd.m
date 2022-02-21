function [y1, y1_norm2, m] = coarseTO_estimator_v2_with_overlapadd(r, q, sps)
% coarseTO_estimator
%   use overlap add method
% 
%     Input variable: r(a vector), q(a vector), sps(a scalar)
%     Output variable: y1(a vector), y1_norm2(a vector), m(a vector) 
%
%     sps:  SamplesPerSymbol
%     m:    course pilot position index of r
% 
%     call function: find_burst_interval.m

% Match filtering r with q
% sol1:

tStart1 = tic;
y1 = filter(conj(q(end : -1 : 1)), 1, r); % or y1 = fftconv(conj(q(end : -1 : 1)), r);
tElapsed1 = toc(tStart1);
%fprintf("convolution without overlap add method is done in %f(sec).\n", tElapsed1);  

%% Test with overlap add method v1
%{
Lb = 1000;
tStart1 = tic;
y1_test = overlapadd_v1(conj(q(end : -1 : 1)), r, Lb);  % number of block
y1_test = y1_test(1:length(r));
tElapsed1 = toc(tStart1);
fprintf("convolution with overlap add method is done in %f(sec).\n", tElapsed1);  
%}
%%  Test with overlap add method v2
%{
Q = 1000; % Q must satisfy L>=Q && Q>=M. % Q = 3000:0.036; Q = 4000: 0.02869; Q = 5000: 0.028569(sec). Q = 6000: 0.027137(sec).  Q = 10000: 0.026402(sec).
h = conj(q(end : -1 : 1));
x = r;
tStart1 = tic;
y1 = overlapadd_v2(h,x,Q);
y1 = y1(1:length(r));
tElapsed1 = toc(tStart1);
fprintf("convolution with overlap add method v2 is done in %f(sec).\n", tElapsed1);  
%}

y1_norm2 = abs(y1) .^ 2; % or y1_energy = abs(y1) .^ 3;
[max_y1, max_y1_index] = max(y1_norm2); 
%figure
%plot(y1_norm2);

% sol1:
%n  = find_high_pilot( y1_norm2 , sps);  % the course pilot position index of y1, which the index corresponding to the local maximum of y1_norm2
% sol2:
burst_interval = find_burst_interval( y1_norm2 , sps); 
% 1st row: head position of the burst(i.e. the pilot position)
% 2nd row: end position of the burst
m  = burst_interval - length(q) + 1;                 % the course pilot position index of r
end