% if IQinterlace = 1: Tx is I leads Q
% if IQinterlace = 2: Tx is Q leads I
function [u_hat, l_hat, b_i_hat, b_q_hat, RxSym] = OQPSK_aftendRx(SigCFOCPOTOcompen, Pulseshaping, FilterSpanInSymbols, sps, IQinterlace)
%% Pulse shaping filter (Rx) 
u_hat = conv(Pulseshaping(end:-1:1), SigCFOCPOTOcompen); % u_hat = filter(Pulseshaping(end:-1:1), 1, SigCFOCPOTOcompen);
% plot
%{
figure 
stem([0 : length(u_hat) - 1] , real(u_hat)); hold on
stem([0 : length(u_hat)-1] , imag(u_hat)); hold on
stem( [0:length(u) + sps*FilterSpanInSymbols- 1] , [zeros(1, sps*FilterSpanInSymbols) real(u(1:end))]);
stem( [0:length(u) + sps*FilterSpanInSymbols- 1] , [zeros(1, sps*FilterSpanInSymbols) imag(u(1:end))]);
title("Received OQPSK signal(after pulse shaping filter)");
legend("Real part", "Imaginary part","Original transmitted Symbol(Real part): $real(\bf{u})$",...
    "Original transmitted Symbol(Imaginary part): $imag(\bf{u})$", 'Interpreter','latex');
%}
%% Down sample by sps/2
l_hat = u_hat(1 : sps/2 : end);
l_hat = l_hat(FilterSpanInSymbols * 2 + 1 : end - FilterSpanInSymbols * 2); 
%% Align I, Q ch, ... and get b_i_hat, b_q_hat 
if IQinterlace == 1      % If Tx: I leads Q
    % Real part
    b_i_hat = real(l_hat(1:2:end));
    % Imaginary part
    b_q_hat = imag(l_hat(2:2:end));
elseif IQinterlace == 0  % If Tx: Q leads I
    % Real part
    b_i_hat = real(l_hat(2:2:end));
    % Imaginary part
    b_q_hat = imag(l_hat(1:2:end));
end
% plot
%{
figure 
stem( [0:length(b_i_hat) - 1] , b_i_hat); 
title("$\hat{b}_i[n]$", 'Interpreter', 'latex');

figure 
stem( [0:length(b_q_hat) - 1] , b_q_hat); 
title("$\hat{b}_q[n]$", 'Interpreter', 'latex');

figure
z = zeros(1, length(b_i_hat));
scatter(b_i_hat, z, 'r.'); hold on
scatter(z, b_q_hat, 'b.');
%xlim([-8 8]);
%ylim([-8 8]);
xlabel("$\hat{b_i}[n]$", "Interpreter", "latex");
ylabel("$\hat{b_q}[n]$", "Interpreter", "latex");

for i = 1:length(b_i_hat)
    s = {' ',num2str(i)};
    text(b_i_hat(i), z(i), num2str(i));
end
for i = 1:length(b_q_hat)
    text(z(i), b_q_hat(i), num2str(i));
end
%}
%% Get the received symbol
RxSym_ = zeros(1,length(b_i_hat)*2);
RxSym_(1:2:end) = b_i_hat;
RxSym_(2:2:end) = b_q_hat;
%% Normalize the received symbol
RxSym = RxSym_/max(RxSym_);
% plot
%{
figure 
index = [0:length(real(RxSym_normalized))-1];
stem(index,RxSym_normalized); 
ylim([-1.5,1.5]);
title("Received Symbol(after normalized)");
%}
end