#include <stdio.h> // Inicializo libreria para imprimir por pantalla
#include <time.h>  // Inicializo libreria para calcular los tiempos
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *t1();
void *t2();
void *t3();

int main(void)
{
    pthread_t tarea1;
    pthread_t tarea2;
    pthread_t tarea3;

    pthread_create(&tarea1, NULL, t1, NULL);
    pthread_create(&tarea2, NULL, t2, NULL);
    pthread_create(&tarea3, NULL, t3, NULL);
    pthread_join(tarea1, NULL);
    pthread_join(tarea2, NULL);
    pthread_join(tarea3, NULL);

    return 0;
}

void *t1()
{
    clock_t inicio = clock(); // Guardo la cantidad de clocks transcurridos
    for (;;)
    {
        float tiempo = (((double)(clock() - inicio) / (CLOCKS_PER_SEC)));

        if (tiempo >= 0.1) // cuento 100 milisegundos
        {
            printf("Tarea 1\n");
            printf("Tiempo de tarea 1 en milisegundos: %lf\n", tiempo * 1000);
            inicio = clock();
        }
    }

    return NULL;
}

void *t2()
{
    clock_t inicio = clock(); // Guardo la cantidad de clocks transcurridos
    for (;;)
    {
        float tiempo = (((double)(clock() - inicio) / (CLOCKS_PER_SEC)));

        if (tiempo >= 0.3) // cuento 300 milisegundos
        {
            printf("Tarea 2\n");
            printf("Tiempo de tarea 2 en milisegundos: %lf\n", tiempo * 1000);
            inicio = clock();
        }
    }

    return NULL;
}

void *t3()
{
    clock_t inicio = clock(); // Guardo la cantidad de clocks transcurridos
    for (;;)
    {
        float tiempo = (((double)(clock() - inicio) / (CLOCKS_PER_SEC)));

        if (tiempo >= 0.5) // cuento 500 milisegundos
        {
            printf("Tarea 3\n");
            printf("Tiempo de tarea 3 en milisegundos: %lf\n", tiempo * 1000);
            inicio = clock();
        }
    }

    return NULL;
}

//https://www.um.es/earlyadopters/actividades/a3/PCD_Activity3_Session1.pdf