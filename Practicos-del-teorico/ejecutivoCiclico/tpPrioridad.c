#include <stdio.h> // Inicializo libreria para imprimir por pantalla
#include <time.h>  // Inicializo libreria para calcular los tiempos
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>

void t1(float tiempo);
void t2(float tiempo);
void t3(float tiempo);

int main(void)
{

    clock_t tiempo1 = clock(); // Guardo la cantidad de clocks transcurridos
    clock_t tiempo2 = clock(); // Guardo la cantidad de clocks transcurridos
    clock_t tiempo3 = clock(); // Guardo la cantidad de clocks transcurridos
    float tiempo;
    for (int i = 0;; i++)
    {

        tiempo = (((double)(clock() - tiempo1) / (CLOCKS_PER_SEC)));
        if (tiempo >= 0.1)
        {
            t1(tiempo);
            t1(tiempo);
            t1(tiempo);
            tiempo1 = clock();
        }

        tiempo = (((double)(clock() - tiempo2) / (CLOCKS_PER_SEC)));
        if (tiempo >= 0.3)
        {
            t2(tiempo);
            t2(tiempo);
            tiempo2 = clock();
        }

        tiempo = (((double)(clock() - tiempo3) / (CLOCKS_PER_SEC)));
        if (tiempo >= 0.5)
        {
            t3(tiempo);
            tiempo3 = clock();
        }
    }

    return 0;
}

void t1(float tiempo)
{
    printf("Tarea 1\n");
    printf("Tiempo de tarea 1 en milisegundos: %lf\n", tiempo * 1000);

    return;
}

void t2(float tiempo)
{
    printf("Tarea 2\n");
    printf("Tiempo de tarea 2 en milisegundos: %lf\n", tiempo * 1000);

    return;
}

void t3(float tiempo)
{
    printf("Tarea 3\n");
    printf("Tiempo de tarea 3 en milisegundos: %lf\n", tiempo * 1000);

    return;
}