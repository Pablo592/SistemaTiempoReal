#include <stdio.h> // Inicializo libreria para imprimir por pantalla
#include <time.h>  // Inicializo libreria para calcular los tiempos
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>

void *t1();
void *t2();
void *t3();

int main(void)
{
    pthread_t tarea1; //
    pthread_t tarea2;       //Inicializo los hilos
    pthread_t tarea3; //

    struct sched_param params1, params2, params3;   //Creo estructuras para almacenar las prioridades


    pthread_create(&tarea1, NULL, t1, NULL); //
    pthread_create(&tarea2, NULL, t2, NULL);    // Creo los hilos y establezco las funciones t1,t2 y t3 como las tareas que van a realizar
    pthread_create(&tarea3, NULL, t3, NULL); //   

    // En GNU ubuntu la prioridad de los hilos va de 1 a 99

    params1.sched_priority = 99;                        // Establezco la prioridad del hilo
    pthread_setschedparam(tarea1, SCHED_RR, &params1);  //Declaro que van a utilizar Round Robin

    params2.sched_priority = 1;                        // Establezco la prioridad del hilo
    pthread_setschedparam(tarea2, SCHED_RR, &params2);  //Declaro que van a utilizar Round Robin

    params3.sched_priority = 1;                       // Establezco la prioridad del hilo
    pthread_setschedparam(tarea3, SCHED_RR, &params3); //Declaro que van a utilizar Round Robin

    pthread_join(tarea1, NULL); //
    pthread_join(tarea2, NULL);     // Espero a que el hilo finalice su ejecucion
    pthread_join(tarea3, NULL); //

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

/*
SCHED_FIFO: este planificador asigna una prioridad fija a cada hilo, y los hilos se ejecutan en orden de prioridad. 
Si un hilo de mayor prioridad se vuelve listo para ejecutarse, se ejecuta inmediatamente, interrumpiendo cualquier 
hilo de menor prioridad que esté en ejecución.

SCHED_RR: este planificador asigna una prioridad fija a cada hilo, pero los hilos se ejecutan en rondas. 
Cada hilo tiene un límite de tiempo asignado para ejecutarse (quantum), después del cual se suspende y se 
cede el control al siguiente hilo de la misma prioridad o inferior.

SCHED_OTHER: este planificador es el predeterminado en la mayoría de las distribuciones de Linux. 
Asigna prioridades dinámicamente en función de la carga del sistema y otros factores. Este planificador 
no permite establecer prioridades específicas.
*/