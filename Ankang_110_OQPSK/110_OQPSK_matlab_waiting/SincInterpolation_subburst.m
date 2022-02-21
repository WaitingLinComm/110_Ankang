function [ResampleSig_buffer] = SincInterpolation_subburst(InSig, m, cpu, id, window)
% SincInterpolation_subburst
%
% Divide InSig into several subbursts and implement SincInterpolation separately. 
% 
% [Input variable ]: InSig(a vector), id(a scalar), cpu(a scalar), window(a scalar)
% [Output variable]: ResampleSig_buffer(a vector)

    SubBurstLength = 1e6;
    numberofBurst = ceil(length(InSig) / SubBurstLength); 
    TailSubBurstLength = mod(length(InSig), SubBurstLength);

    ResampleSig_buffer = [];
    for i = 1 : numberofBurst
    if i == numberofBurst
        ResampleSig = zeros(1, fix(TailSubBurstLength * m));
    else   
        ResampleSig = zeros(1, fix(SubBurstLength * m));
    end

    for n2 = id : cpu :  fix(SubBurstLength * m)
        inner_sum = 0;
        n1_match = round(n2 / m); % 由 n2 對應到 n1 的index
        if n1_match > window & n1_match < length(InSig) - window       
            for n1 = n1_match - window : n1_match + window
                inner_sum = inner_sum + InSig(n1) * sinc(n2/m - n1);
            end
            ResampleSig(n2) = inner_sum;
        elseif n1_match <= window
            for n1 = 1 : n1_match + window
                inner_sum = inner_sum + InSig(n1) * sinc(n2/m - n1);
            end
            ResampleSig(n2) = inner_sum;
        elseif n1_match >= window
            for n1 = n1_match - window : length(InSig)
                inner_sum = inner_sum + InSig(n1) * sinc(n2/m - n1);
            end
            ResampleSig(n2) = inner_sum;
        end
    end
    ResampleSig_buffer = [ResampleSig_buffer ResampleSig];
    end
end