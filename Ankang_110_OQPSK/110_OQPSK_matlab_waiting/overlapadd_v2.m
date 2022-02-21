function y=overlapadd_v2(h,x,Q) % Q: one circular convolution block length
% overlapadd_v2
%
% Ref: https://alan23273850.gitbook.io/signals-and-systems/di-wu-zhang-li-san-fu-li-ye-zhuan-huan-dft/di-liu-jie-dft-de-ying-yong-san-ji-suan-da-gui-mo-shi-yong-de-xian-xing-zhe-ji

    L=length(x); M=length(h);
    if ~(L>=Q && Q>=M) % check validity of Q
        error('Q must satisfy L>=Q && Q>=M.');
    end
    P=Q-M+1; % Overlap
    Nblocks=ceil((L-Q)/P+1);
    y=zeros(1,L+M-1);
 
    z=conv(x(1:Q),h);
    y(1:Q)=z(1:Q);
 
    for m=1:Nblocks-2
        z=cconv(h,x(m*P+(1:Q)),Q);
        y(Q+(m-1)*P+(1:P))= z(M:Q);
    end
    m=Nblocks-1;
    z=conv(h,x(m*P+1:end));
    y(Q+(m-1)*P+1:end)=z(M:end);
end