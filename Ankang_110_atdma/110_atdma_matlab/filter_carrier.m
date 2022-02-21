function y = filter_carrier( x, Ts, f_high, f_low )
N=length(x);                               % length of the signal x
t=Ts*(1:N);                                % define a time vector
ssf=(ceil(-N/2):ceil(N/2)-1)/(Ts*N);       % frequency vector , [-0.5fs, 0.5fs] 
filter = (ssf < f_high) + (ssf > f_low) -1 ; 
fx=fft(x(1:N));                            % do DFT/FFT
fxs=fftshift(fx);                          % shift it for plotting
y = fftshift(ifft(ifftshift(filter.*fxs)));   %Invrse Fourier Transform
end