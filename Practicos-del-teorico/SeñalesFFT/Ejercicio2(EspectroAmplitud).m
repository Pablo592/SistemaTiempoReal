t = -10:0.00001:10;
a = input("Ingrese la amplitud");
f = input("Ingrese la frecuencia");
y = a*sin(2*pi*f*t); 
s = fft(y);
plot(s);
title('Espectro de amplitud');
grid on