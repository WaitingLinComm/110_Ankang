%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% sinc.m
% same as Matlab's sinc function
% sinc(x) =  sin(pi*x)/(pi*x)
% Last updated 8/31/00 by K. Bell
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function y=sinc(x)
for i=1:length(x)
    if x(i)==0
        y(i)=1;
    else
        y(i)=sin(pi*x(i))./(pi*x(i));
    end
end