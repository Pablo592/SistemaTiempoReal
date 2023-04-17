#include <wiringPi.h> // Inicializo libreria para controlar los pines
#include <stdio.h>	  // Inicializo libreria para imprimir por pantalla
#include <time.h>	  // Inicializo libreria para calcular los tiempos

#define LED 17 // El pin donde se conecta el Led

int main(void)
{
	clock_t start_time; // Declaro variable para guardar los clocks
	clock_t end_time; // Declaro variable para guardar los clocks
	
	wiringPiSetupGpio(); // Establezco conexion con los pines
	pinMode(LED, INPUT); // Declaro al pin 17 como pin de salida
	int pulsador_activo_fl = 0;			 //seteo un flag

	while (1)
	{
		pulsador_activo_fl = digitalRead(LED);	// en 1 esta prendido, en 0 esta apagado
		if (1 == pulsador_activo_fl)
		{
			end_time = clock(); 					//tomo el ultimo tiempo
			pulsador_activo_fl = digitalRead(LED);	//vuelvo a leer el pulsador
		}
		else
		{
			double tiempoTranscurrido = (((double)(clock() - end_time) / (CLOCKS_PER_SEC)));

			start_time = clock();
			while (1 != digitalRead(LED))
			{
				if ((((double)(clock() - start_time) / (CLOCKS_PER_SEC))) <= tiempoTranscurrido) // Voy contando los clocks hasta que pase medio segundo
				{
					digitalWrite(LED, 1); // Prendo el Led
					printf("Prendo\n");	  // Imprimo en pantalla
				}
			}
			digitalWrite(LED, 0);
		}
	}

	return 0;
}