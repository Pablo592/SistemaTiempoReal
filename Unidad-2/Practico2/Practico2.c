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

#define AHT10_ADDRESS 0x38 // AHT10 I2C address
#define VENTILACION 17     // El pin donde se conecta el Led

void *tareaA();
void *tareaB();
void *tareaC();
int file;
float temperatura;
int main(void)
{
    temperatura/*= 50*/;
    pinMode(VENTILACION, OUTPUT); // Declaro al pin 17 como pin de salida
    char file[6];
    pthread_t tarea1; //
    pthread_t tarea2; // Inicializo los hilos
    pthread_t tarea3; //

    pthread_create(&tarea1, NULL, tareaA, NULL); //
    pthread_create(&tarea2, NULL, tareaB, NULL); // Creo los hilos y establezco las funciones t1,t2 y t3 como las tareas que van a realizar
    pthread_create(&tarea3, NULL, tareaC, NULL); //
    pthread_join(tarea1, NULL);                  //
    pthread_join(tarea2, NULL);                  // Espero a que el hilo finalice su ejecucion
    pthread_join(tarea3, NULL); //

    return 0;
}

void *tareaA()
    {
        wiringPiSetupGpio();                     // Establezco conexion con los pines
        char *filename = "/dev/i2c-1";           // I2C bus device file
        if ((file = open(filename, O_RDWR)) < 0) // Verifico la conexion con la interfaz I2C
        {
            printf("Failed to open I2C bus %s\n", filename);
            exit(1);
        }

        if (ioctl(file, I2C_SLAVE, AHT10_ADDRESS) < 0) // Verifico la conexion con el sensor AHT10
        {
            printf("Failed to select AHT10 sensor\n");
            exit(1);
        }

        return NULL;
    }

 void *tareaB()
 {
     float cur_temp;
     clock_t inicio = clock();             // Guardo la cantidad de clocks transcurridos
     char command[3] = {0xAC, 0x33, 0x00}; // Busco en memoria la humedad y temperatura detectada por el sensor

     for (;;)
     {
         float tiempo = (((double)(clock() - inicio) / (CLOCKS_PER_SEC)));

         if (tiempo >= 60) // cuento 1 minuto
         {
             write(file, command, 3);
             usleep(50000); // Se espera un tiempo a que las mediciones puedan completarse (50ms)
             char data[6];
             read(file, data, 6);
             cur_temp = (((data[3] & 0x0F) << 16) | (data[4] << 8) | data[5]);
             cur_temp = ((cur_temp * 200) / 1048576) - 50;
             printf("Temperature: %2.2f\n", cur_temp); // Se imprime la temperatura monitoreada por consola
             temperatura = cur_temp;

             inicio = clock();
         }
     }

     return NULL;
 }

void *tareaC()
{
    clock_t inicio = clock(); // Guardo la cantidad de clocks transcurridos
    struct timeval ti, tf;
    double tiempo_pulsador;

    for (;;)
    {
        float tiempo = (((double)(clock() - inicio) / (CLOCKS_PER_SEC)));

        if (tiempo >= 60) // cuento 500 milisegundos
        {
            if ((temperatura > 30) || (temperatura < 25))
            {

                gettimeofday(&ti, NULL);
                while (((tf.tv_sec - ti.tv_sec)*1000 + (tf.tv_usec - ti.tv_usec)/1000.0)/1000 <= 120)
                {
                    printf("Encender sistema de ventilación\n");

                    printf("%f\n",((tf.tv_sec - ti.tv_sec)*1000 + (tf.tv_usec - ti.tv_usec)/1000.0));
                    gettimeofday(&tf, NULL);
                }
            }
            inicio = clock();
        }
    }

    return NULL;
}

// https://www.um.es/earlyadopters/actividades/a3/PCD_Activity3_Session1.pdf