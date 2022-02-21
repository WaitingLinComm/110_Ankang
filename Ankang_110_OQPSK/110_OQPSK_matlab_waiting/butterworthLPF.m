function [LP_Sig] = butterworthLPF(n, Wn, InSig)
% butterworthLPF
% 
% [Input variable ]: n, Wn, InSig(a vector)
% [Output variable]: LP_Sig(a vector)
% 
% nth order butterworth filter
% cutoff frequency =  Wn * fs 

[b,a] = butter(n, Wn, 'low');
LP_Sig = filter(b, a, InSig);
end
% figure
% freqz(b,a)
% figure
% f  = impz(b,a); % value of impulse response
% name1 = ['butterworthLPF.txt'] ;
% writematrix(f, name1);
% figure;
% plotspec(LP_Sig,Ts);