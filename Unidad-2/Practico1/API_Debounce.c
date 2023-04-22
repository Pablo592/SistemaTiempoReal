#include <wiringPi.h> // Inicializo libreria para controlar los pines
#include <stdio.h>	  // Inicializo libreria para imprimir por pantalla
#include <time.h>	  // Inicializo libreria para calcular los tiempos
#include <sys/time.h>

#define PULSADOR 17 // El pin donde se conecta el PULSADOR
#define LED 18		// El pin donde se conecta el LED

int main(void)
{
	struct timeval ti, tf;
    double tiempo_pulsador;
    double tiempo_inicio_led_prender;
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
			gettimeofday(&ti, NULL);   // Instante inicial en que se presiona el pulsador
			presiono = 1;				
			printf("Presionando Pulsador\n"); 
		}
		else
		{
			if (presiono)
			{
				gettimeofday(&tf, NULL);   // seteamos el tiempo final en que se termindo de presionar el pulsador
				tiempo_pulsador= (tf.tv_sec - ti.tv_sec)*1000 + (tf.tv_usec - ti.tv_usec)/1000.0;	// Obtenemos el tiempo pulsado 
				printf("Has tardado en pulsar : %g Seg\n", tiempo_pulsador/1000);

				tiempo_inicio_led_prender = 0;
				gettimeofday(&ti, NULL);   // Instante inicio del led a prender


				while ((tiempo_inicio_led_prender < tiempo_pulsador))
				{
					digitalWrite(LED, 1); // Prendemos el LED
					gettimeofday(&tf, NULL);   
					tiempo_inicio_led_prender=(tf.tv_sec - ti.tv_sec)*1000 + (tf.tv_usec - ti.tv_usec)/1000.0; //Obtenemos el tiempo que lleva el led prendido
					printf("Prendo led\n");
				}
				printf("El tiempo prendido del led es : %g milisegundos\n", tiempo_inicio_led_prender);
				digitalWrite(LED, 0);
				presiono = 0;
			}
		}
	}

	return 0;
}