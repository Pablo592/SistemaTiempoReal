#include <stdio.h> // Inicializo libreria para imprimir por pantalla
#include <time.h>  // Inicializo libreria para calcular los tiempos
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <wiringPi.h>      //
#include <linux/i2c-dev.h> //
#include <sys/ioctl.h>     //
#include <fcntl.h>         //

double tiempoPulsado;

int main(void)
{


    struct timeval ti, tf;

    pid_t ret = fork();

    if (ret)
    {
        
        double tiempo_pulsador;
       
        int presiono = 0;

        wiringPiSetupGpio();        // Establezco conexion con los pines
        pinMode(PULSADOR, INPUT);   // Declaro al pin 17 como pin de entrada
        pinMode(LED, OUTPUT);       // Declaro al pin 18 como pin de salida
        int pulsador_activo_fl = 0; // seteo un flag

        while (1)
        {
            pulsador_activo_fl = digitalRead(PULSADOR); // en 1 esta prendido, en 0 esta apagado
            if (pulsador_activo_fl && presiono == 0)
            {
                gettimeofday(&ti, NULL); // Instante inicial en que se presiona el pulsador
                presiono = 1;
                printf("Presionando Pulsador\n");
            }
            else
            {
                if (presiono)
                {
                    gettimeofday(&tf, NULL);                                                               // seteamos el tiempo final en que se termindo de presionar el pulsador
                    tiempo_pulsador = (tf.tv_sec - ti.tv_sec) * 1000 + (tf.tv_usec - ti.tv_usec) / 1000.0; // Obtenemos el tiempo pulsado
                    printf("Has tardado en pulsar : %g Seg\n", tiempo_pulsador / 1000);

                    tiempoPulsado = tiempo_pulsador / 1000;
                    presiono = 0;
                }
            }
        }
    }
    else
    {
         double tiempo_inicio_led_prender;
        for (;;)
        {
            tiempo_inicio_led_prender = 0;
            gettimeofday(&ti, NULL); // Instante inicio del led a prender

            while ((tiempo_inicio_led_prender < tiempoPulsado))
            {
                digitalWrite(LED, 0);
                gettimeofday(&tf, NULL);
                tiempo_inicio_led_prender = (tf.tv_sec - ti.tv_sec) * 1000 + (tf.tv_usec - ti.tv_usec) / 1000.0; // Obtenemos el tiempo que lleva el led prendido
            }
            digitalWrite(LED, 1); // Prendemos el LED
            printf("El periodo del LED es de : %g milisegundos\n", tiempo_inicio_led_prender);
        }
    }

    return 0;
}