#include <wiringPi.h> // Inicializo libreria para controlar los pines
#include <stdio.h>	  // Inicializo libreria para imprimir por pantalla
#include <time.h>	  // Inicializo libreria para calcular los tiempos

#define PULSADOR 17 // El pin donde se conecta el PULSADOR
#define LED 18		// El pin donde se conecta el LED

int main(void)
{
	clock_t start_time_led;		 // Declaro variable para guardar los clocks
	clock_t start_time_pulsador; // Declaro variable para guardar los clocks
	int presiono = 0;

	wiringPiSetupGpio();		// Establezco conexion con los pines
	pinMode(PULSADOR, INPUT);	// Declaro al pin 17 como pin de entrada
	pinMode(LED, OUTPUT);		// Declaro al pin 18 como pin de salida
	int pulsador_activo_fl = 0; // seteo un flag

	while (1)
	{
		pulsador_activo_fl = digitalRead(PULSADOR); // en 1 esta prendido, en 0 esta apagado
		if (pulsador_activo_fl && presiono == 0)
		{
			start_time_pulsador = clock(); // tomo el ultimo tiempo
			presiono = 1;
		}
		else
		{
			double tiempoTranscurrido = (((double)(clock() - start_time_pulsador) / (CLOCKS_PER_SEC))); // Se calcula el delta del tiempo transcurrido entre que presiono y suelto el pulsador

			start_time_led = clock();
			if (presiono)
			{
				while (((((double)(clock() - start_time_led) / (CLOCKS_PER_SEC))) <= tiempoTranscurrido))
				{
					digitalWrite(LED, 1); // Prendo el PULSADOR
					printf("Prendo\n");	  // Imprimo en pantalla
				}
				digitalWrite(LED, 0);
				presiono == 0;
				start_time_pulsador = clock(); // receteo el tiempo del pulsador
			}
		}
	}

	return 0;
}