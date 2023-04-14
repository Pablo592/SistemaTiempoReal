t = -10:0.00001:10;
a = input("Ingrese la amplitud");
f = input("Ingrese la frecuencia");
y = a*sin(2*pi*f*t) + a*sin(2*pi*(2*f)*t); 
s = fft(y);
plot(fftshift(imag(s)));
title('Espectro de fase');
grid on