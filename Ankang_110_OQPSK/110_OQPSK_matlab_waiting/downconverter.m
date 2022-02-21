function [Sig_demod] = downconverter(InSig, f_c, Ts)

Sig_demod = InSig .* exp(-1i * 2 * pi * Ts * f_c * (1 : length(InSig)));