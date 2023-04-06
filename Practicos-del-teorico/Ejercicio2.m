t = -10:0.00001:10;
a = input("Ingrese la amplitud");
f = input("Ingrese la frecuencia");
y = a*sin(2*pi*f*t);
plot(t,y,'ro');
xlabel('Eje de tiempo');
ylabel('Eje de amplitud'); 
title('Grafica de la funcion senoseidal');
grid on