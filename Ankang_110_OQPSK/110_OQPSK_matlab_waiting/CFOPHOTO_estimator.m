function [cfo_hat, pho_hat, d, y2_norm2_buffer, vCFO_buffer] = CFOPHOTO_estimator(v, q, K, Ts)
% CFOPHOTO_estimator.m
% 
% [Input variable ]: v(a vector), q(a vector), K, Ts
% [Output variable]: cfo_hat(a scalar), pho_hat(a scalar), d(a scalar), y2_norm2_buffer(a vector), vCFO_buffer (a vector)
%
% NOTE: 
% (1) Total number of processed data in CFOPHOTO_estimator is 2*K + 1, i.e. length(v) = 2*K + 1. 
%     After finding the course position of the pilot in coarseTO_estimator, we select its front and back K samples as the processed data in CFOPHOTO_estimator
%     當找到 pilot 的位置後，取其位置的前 K 與後 K 個sample(共2K + 1個點) 當做 CFOPHOTO_estimator 的輸入
% (2) d: the time index difference found by the CFOPHOTO_estimator and Coarse TO esitmaion 
% (3) cfo_hat, pho_hat: [-pi ~ pi] (rad)

%% Buffer initialization
cfo_estimate_buffer = []; 
pho_estimate_buffer = [];
to_estimate_buffer = [];
pho_observing_matrix_buffer = {};
y2_norm2_buffer = {};
vCFO_buffer = {};
%% Parameter setting
cfo_estimate = 0;
cfo_step = 1000;
nSteps = 10;
iterations = 0;

while(cfo_step > 1e-7)
    iterations = iterations + 1;
    vCFO = cfo_estimate + (-nSteps : 1 : nSteps) * cfo_step; % Hz    
    y2_norm2 = zeros(length(vCFO), length(v));
    pho = zeros(length(vCFO), length(v));
    CFO = zeros(1, length(vCFO));
    for i =1:length(vCFO)
        CFO(i) = 2 * pi * vCFO(i) * Ts; % CFO:[-pi,pi] (rad)
        p = q .* exp(1i * CFO(i) * (1 : length(q)));
        y2 = filter(conj(p(end : -1 : 1)), 1, v); % or y2 = fftconv((v),conj(p(end:-1:1)));     
        y2_norm2(i,:) = abs(y2).^2;
        pho(i,:) = angle(y2);
    end

    [M,I] = max(y2_norm2(:));  % I is the index maximun.
    [cfo_estimate_index, to_estimate] = ind2sub(size(y2_norm2), I);
    if size(cfo_estimate_index,1) > 1 || size(cfo_estimate_index,1) > 1
        cfo_estimate_index = cfo_estimate_index(1);
        to_estimate = to_estimate(1);
    end
   
    cfo_step = cfo_step/nSteps*2;
    %% Buffer
    y2_norm2_buffer{iterations} = y2_norm2;
    vCFO_buffer{iterations} = vCFO;
     
    cfo_estimate = vCFO(cfo_estimate_index);
    cfo_estimate_buffer = [cfo_estimate_buffer cfo_estimate];
    
    pho_estimate = pho(cfo_estimate_index,to_estimate);
    pho_estimate_buffer = [pho_estimate_buffer pho_estimate];
    pho_observing_matrix_buffer{iterations} = pho;
    
    to_estimate_buffer = [to_estimate_buffer to_estimate];
end
final_to_estimate = to_estimate_buffer(end); 
n2 = final_to_estimate - length(q) + 1;          % index of v (for the corresponding time index of the highset peak after FineTOPHOCFO estimation)
n1 = K + 1;                                      % index of v (for the corresponding time index of the highset peak after Coarse TO esitmaion)
d = n2 - n1; 
cfo_hat = 2 * pi* cfo_estimate_buffer(end) * Ts; % rad,  [-pi, pi]
pho_hat = pho_estimate_buffer(end);              % rad

cfo_estimate_buffer;
pho_estimate_buffer;
to_estimate_buffer;
end