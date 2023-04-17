#include <wiringPi.h> // Inicializo libreria para controlar los pines
#include <stdio.h>	  // Inicializo libreria para imprimir por pantalla
#include <time.h>	  // Inicializo libreria para calcular los tiempos

#define LED 17 // El pin donde se conecta el Led

int main(void)
{
	clock_t start; // Declaro variable para guardar los clocks
	int i = 0;	   // Declaro variable hacer el while infinito

	wiringPiSetupGpio(); // Establezco conexion con los pines
	pinMode(LED, INPUT); // Declaro al pin 17 como pin de salida
	int b = 0;

	while (1 > i)
	{
		if (1 == digitalRead(LED))
		{
			if (start == NULL)
				start = clock();
		}
		else
		{
			double tiempoTranscurrido = (((double)(clock() - start) / (CLOCKS_PER_SEC)));
			start = clock();
			while (1 != digitalRead(LED))
			{
				if ((((double)(clock() - start) / (CLOCKS_PER_SEC))) <= tiempoTranscurrido) // Voy contando los clocks hasta que pase medio segundo
				{
					digitalWrite(LED, 1); // Prendo el Led
					printf("Prendo\n");	  // Imprimo en pantalla
				}
			}
			digitalWrite(LED, 0);
			start = NULL;
		}
	}

	return 0;
}