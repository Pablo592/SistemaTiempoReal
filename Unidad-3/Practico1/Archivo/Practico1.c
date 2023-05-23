#include <stdio.h> // Inicializo libreria para imprimir por pantalla
#include <time.h>  // Inicializo libreria para calcular los tiempos
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
// #include <wiringPi.h>      //
#include <linux/i2c-dev.h> //
#include <sys/ioctl.h>     //
#include <fcntl.h>         //
#include <stdint.h>
#include <sys/mman.h>
// #include <sys/wait.h>

char *ptr_fecha_hora = NULL;
char *ptr_name = NULL;

double tiempoPulsado;

int main(void)
{

    ptr_name;
    ptr_fecha_hora;

    struct timeval ti, tf;

    pid_t ret = fork();

    int pulsador_activo_fl;
    // printf("Introduzca 1 = PULSADO, 0 <> NO PULSADO: ");
    // scanf("%d", &pulsador_activo_fl);

    if (ret)
    {

        double tiempo_pulsador;

        int presiono = 0;
        /*
        wiringPiSetupGpio();        // Establezco conexion con los pines
        pinMode(PULSADOR, INPUT);   // Declaro al pin 17 como pin de entrada
        pinMode(LED, OUTPUT);       // Declaro al pin 18 como pin de salida


        */
        int pulsador_activo_fl = 0; // seteo un flag
        while (1)
        {
            printf("Introduzca 1 = PULSADO, 0 <> NO PULSADO:\n");
            scanf("%d", &pulsador_activo_fl);
            /*pulsador_activo_fl = digitalRead(PULSADOR);*/ // en 1 esta prendido, en 0 esta apagado
            if (pulsador_activo_fl && presiono == 0)
            {
                gettimeofday(&ti, NULL); // Instante inicial en que se presiona el pulsador
                presiono = 1;
                printf("Presionando Pulsador\n");
                pulsador_activo_fl = 1;
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

                    time_t t = time(NULL);
                    struct tm tm = *localtime(&t);
                    

                    FILE *fichero;
                    fichero = fopen("archivo.config", "na");
                    fprintf(fichero, "%02d:%02d:%02d  %d-%02d-%02d \n%f \nBOTON 1", tm.tm_hour, tm.tm_min, tm.tm_sec, tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tiempoPulsado);
                    fclose(fichero);
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
            // if(*tiempo_pulsado >0)

            FILE *file_pointer;                          // Declaro un puntero que va a ir apuntando al archivo fila por fila
            char str[150];                               // Declaro una variable que va a contener las filas del archivo
            file_pointer = fopen("archivo.config", "r"); // Apunto el puntero al inicio del archivo

            if (file_pointer == NULL)
            {
                printf("Error opening file!");
                exit(1);
            }
            int n = 0;
            float tiempo_pulsado;
            char fecha[150];
            char boton[150];
            while (fgets(str, 150, file_pointer) != NULL)
            {
                n++;
                if (n == 1)
                for (int i = 0; i < strlen(str); i++)
                {
                    fecha[i] = str[i];
                }
                
                if (n == 2)
                    tiempo_pulsado = atof(str);


                if (n == 3)
                for (int i = 0; i < strlen(str); i++)
                {
                    boton[i] = str[i];
                }

            } // Voy recorriendo el archivo fila por fila

            if (n < 2)
                tiempo_pulsado = 0;

            while ((tiempo_inicio_led_prender < tiempo_pulsado))
            {
                /*digitalWrite(LED, 0);*/
                gettimeofday(&tf, NULL);
                tiempo_inicio_led_prender = (tf.tv_sec - ti.tv_sec) * 1000 + (tf.tv_usec - ti.tv_usec) / 1000.0; // Obtenemos el tiempo que lleva el led prendido
            }
            if (tiempo_pulsado != 0)
            {
                fprintf(stdout, "El tiempo pulsado %f.\n", tiempo_pulsado);
                fprintf(stdout, "La fecha es: %s\n", fecha);
                fprintf(stdout, "El nombre del boton: %s\n", boton);
                
                printf("Vuelva a pulsar \n");
            }
            /*digitalWrite(LED, 1);*/ // Prendemos el LED
            // printf("El periodo del LED es de : %g milisegundos\n", tiempo_inicio_led_prender);

            FILE *fichero;
            fichero = fopen("archivo.config", "wt");
            fprintf(fichero, " ");
            fclose(fichero);
            tiempo_pulsado = 0;
        }
    }
    return 0;
}