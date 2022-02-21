function [ResampleSig] = SincInterpolation(InSig, N, cpu, id, window)
%SincInterpolation
% 
%   Input variable: InSig(a vector),  N(a scalar), cpu(a scalar), id(a scalar), window(a scalar)
%   Output variable: ResaNpleSig(a vector)
%
%   N = new SamplePerSymbol / old SamplePerSymbol
%   upsample by N. NOTE: N can be a non-integer number
%
%   cpu: cpu core of computer
%   id: id of cpu core
%   window: half-sided window length(used in SincInterpolation) 
    
    ResampleSig = zeros(1, fix(length(InSig) * N));
    for n2 = id : cpu :fix(length(InSig) * N)    
        inner_sum = 0;
        n1_match = round(n2 / N); % 由 n2 對應到 n1 的index
        if n1_match > window & n1_match < length(InSig) - window       
            for n1 = n1_match - window : n1_match + window
                inner_sum = inner_sum + InSig(n1) * sinc(n2/N - n1); % sin(pi * (n2/N - n1)) / (pi * (n2/N - n1));
                %inner_sum = inner_sum + InSig(n1) * sin(pi * (n2/N - n1)) / (pi * (n2/N - n1));
            end 
            ResampleSig(n2) = inner_sum;
        elseif n1_match <= window
            for n1 = 1 : n1_match + window
                inner_sum = inner_sum + InSig(n1) * sinc(n2/N - n1);
                %inner_sum = inner_sum + InSig(n1) * sin(pi * (n2/N - n1)) / (pi * (n2/N - n1));
            end
            ResampleSig(n2) = inner_sum;
        elseif n1_match >= window
            for n1 = n1_match - window : length(InSig)
                inner_sum = inner_sum + InSig(n1) * sinc(n2/N - n1);
                %inner_sum = inner_sum + InSig(n1) * sin(pi * (n2/N - n1)) / (pi * (n2/N - n1));
            end
            
            ResampleSig(n2) = inner_sum;
        end
    end  
end