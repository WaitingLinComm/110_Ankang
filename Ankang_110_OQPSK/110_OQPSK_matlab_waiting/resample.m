function [ResampleSig] = resample(InSig, N, cpu, window)
% resample
% 
% [Input variable ]: InSig(a vector),  N(a scalar), cpu(a scalar), window(a scalar)
% [Output variable]: ResampleSig(a vector)
% 
% N = new SamplePerSymbol / old SamplePerSymbol
% upsample by N. NOTE: N can be a non-integer number
% 
% cpu: cpu core of computer
% window: half-sided window length(used in SincInterpolation) 
% 
% call function: SincInterpolation

% sol1: faster(with parallel processing)
    parfor (id = 1 : cpu, cpu)    
        % Method 1:
        [subResampleSig(id,:)] = SincInterpolation(InSig, N, cpu, id, window);
        % Method 2:
        % [subResampleSig(id,:)] = SincInterpolation_subburst(InSig, N, cpu, id, window); 
    end
    ResampleSig = sum(subResampleSig);

% sol2: slower(without parallel processing)
%{
    id = 1;
    cpu = 1;
    ResampleSig = SincInterpolation(InSig, N, cpu, id, window);
%}
end    


