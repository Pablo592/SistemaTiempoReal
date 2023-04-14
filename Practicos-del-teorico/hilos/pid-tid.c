#include <stdio.h> // Inicializo libreria para imprimir por pantalla
#include <time.h>  // Inicializo libreria para calcular los tiempos
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *t1();

int main(void)
{
    pthread_t tarea1; //

    pthread_create(&tarea1, NULL, t1, NULL); //
    pthread_join(tarea1, NULL);              //

    return 0;
}

void *t1()
{
    pid_t pid;
    pthread_t tid;

    pid = getpid();
    tid = pthread_self();

    printf("PID del proceso que contiene al hilo: %d\n", (int)pid);
    printf("TID del hilo: %lu\n", (unsigned long)tid);

    return NULL;
}
// https://www.um.es/earlyadopters/actividades/a3/PCD_Activity3_Session1.pdf
/*
Codifique  un  programa  que  genere  un  hilo y desde  la función
complementaria imprima por pantalla su PID y TID.
*/

/*int pthread_create(pthread_t* thread, const pthread_attr_t* attr, void* (*start_routine)(void*), void* arg);*/

/*
thread: un puntero a una variable pthread_t que se utilizará para almacenar el identificador del hilo creado.
attr: un puntero a una estructura pthread_attr_t que se utiliza para especificar los atributos del hilo creado. Si se pasa NULL, se utilizan los atributos predeterminados.
start_routine: un puntero a la función que se ejecutará en el hilo creado. Esta función debe tomar un argumento void* y devolver un valor void*.
arg: un puntero al argumento que se pasará a la función start_routine().

*/

/*
Cada proceso tiene un identificador de proceso (PID) único, que se utiliza para identificar el proceso en el sistema operativo.
Cada hilo dentro de un proceso tiene su propio identificador de hilo (TID) único, que se utiliza para identificar el hilo en el sistema operativo. 
*/