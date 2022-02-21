
function [LP_Sig] = chebyshev(n, Rp, Wp, InSig)
% nth-order lowpass digital Chebyshev Type I filter 
% Rp: peak-to-peak passband ripple.
% Wp: normalized passband edge frequency

[b,a] = cheby1(n, Rp, Wp,'low');
LP_Sig = filter(b, a, InSig);
end
% figure
% freqz(b,a)
% figure
% impz(b,a)
% figure;
% plotspec(LP_Sig,Ts);