function [U] = PHO_aftendRx_Optimization(l_hat, l)
a = real(l_hat(1:2:end));
b = imag(l_hat(1:2:end));
c = real(l_hat(2:2:end));
d = imag(l_hat(2:2:end));

e = real(l(1:2:end));
f = imag(l(1:2:end));
g = real(l(2:2:end));
h = imag(l(2:2:end));

A = a * b.' - c * d.';
B = a * e.' + d * h.';
C = a * a.' + d * d.';
D = b * b.' + c * c.';
E = -b * e.' + c * h.';
F = c * d.' - a * b .';

x = (-B * D + (C * D * E / F))/(A * F - C * D) + E / F;
y = (- B * F + C * E)/(A * F - C * D);
U = x + j * y;
end