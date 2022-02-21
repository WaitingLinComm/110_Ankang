function [SER_theory, BER_theory] = SERBER_TheoreticalCurve(mode_type, indexSNRdB)
%SERBER_TheoreticalCurve
% 
%   Input variable: mode_type, indexSNRdB(a vector)
%   Output variable: SER_theory(a vector), BER_theory(a vector)
%
%   mod_type : "QPSK", "8PSK", "OQPSK"

SNRlin = 10 .^ (indexSNRdB / 10);

if mode_type == "QPSK" || mode_type == "OQPSK"
    M = 4; 
    k = log2(M);
    %% QPSK  theoratical SER and BER curve 
    % Johng, Proakis, 2014. Communication Systems Engineering (2nd Edition)
    % Ref: Proakis p.448 eq. 8.7.16
    BER_theory = 1 / 2 * erfc(sqrt(SNRlin));
    SER_theory = 2 * BER_theory - BER_theory .^ 2;
    
elseif mode_type == "8PSK"
    M = 8;
    k = log2(M);
    %% MPSK  theoratical SER and BER curve (approximately calculation by using Union Bound to )
    % Johng, Proakis, 2014. Communication Systems Engineering (2nd Edition)
    % Ref: Proakis p.416 eq. 8.6.32
    SER_theory = erfc(sqrt(k*SNRlin)*sin(pi/M)); %or SER_theory = 2*qfunc(sqrt(2*k*SNRlin)*sin(pi/M));
    % Ref: Proakis p.416 eq. 8.6.33
    BER_theory = 1/k.*SER_theory; 

end  
end