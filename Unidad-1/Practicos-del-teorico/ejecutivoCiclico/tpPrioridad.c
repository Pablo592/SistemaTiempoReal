#include <stdio.h> // Inicializo libreria para imprimir por pantalla
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sched.h>
#include <wiringPi.h> // Inicializo libreria para controlar los pines

#define LED100 17 // El pin donde se conecta el Led
#define LED300 18 // El pin donde se conecta el Led
#define LED500 19 // El pin donde se conecta el Led

void t1(float tiempo, bool led1);
void t2(float tiempo, bool led2);
void t3(float tiempo, bool led3);
void prenderLed(int pin, bool estado);

int main(void)
{

    bool led1;
    bool led2;
    bool led3;

    led1 = true;
    led2 = true;
    led3 = true;
    clock_t tiempo1 = clock(); // Guardo la cantidad de clocks transcurridos
    clock_t tiempo2 = clock(); // Guardo la cantidad de clocks transcurridos
    clock_t tiempo3 = clock(); // Guardo la cantidad de clocks transcurridos
    float tiempo;
    for (int i = 0;; i++)
    {

        tiempo = (((double)(clock() - tiempo1) / (CLOCKS_PER_SEC)));
        if (tiempo >= 0.1)
        {
            t1(tiempo, led1);
            led1 = !led1;
            t1(tiempo, led1);
            led1 = !led1;
            t1(tiempo, led1);
            led1 = !led1;
            tiempo1 = clock();
        }

        tiempo = (((double)(clock() - tiempo2) / (CLOCKS_PER_SEC)));
        if (tiempo >= 0.3)
        {
            t2(tiempo,led2);
            led2 = !led2;
            t2(tiempo,led2);
            led2 = !led2;
            tiempo2 = clock();
        }

        tiempo = (((double)(clock() - tiempo3) / (CLOCKS_PER_SEC)));
        if (tiempo >= 0.5)
        {
            t3(tiempo,led3);
            led3 = !led3;
            tiempo3 = clock();
        }
    }

    return 0;
}

void t1(float tiempo, bool led1)
{
    printf("Tarea 1\n");
    printf("Tiempo de tarea 1 en milisegundos: %lf\n", tiempo * 1000);
    prenderLed(LED100, led1);
    return;
}

void t2(float tiempo, bool led2)
{
    printf("Tarea 2\n");
    printf("Tiempo de tarea 2 en milisegundos: %lf\n", tiempo * 1000);
    prenderLed(LED300, led2);

    return;
}

void t3(float tiempo, bool led3),
{
    printf("Tarea 3\n");
    printf("Tiempo de tarea 3 en milisegundos: %lf\n", tiempo * 1000);
    prenderLed(LED500, led3);
    return;
}

void prenderLed(int pin, bool estado)
{
    wiringPiSetupGpio();       // Establezco conexion con los pines
    pinMode(pin, OUTPUT);      // Declaro al pin 17 como pin de salida
    digitalWrite(pin, estado); // Apago el Led
}