#include <stdio.h> // Inicializo libreria para imprimir por pantalla
#include <time.h>  // Inicializo libreria para calcular los tiempos
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
// #include <wiringPi.h>      //
#include <linux/i2c-dev.h> //
#include <sys/mman.h>
#include <sys/ioctl.h> //
#include <fcntl.h>     //
#include <errno.h>
#include <mqueue.h>

#define AHT10_ADDRESS 0x38 // AHT10 I2C address
#define VENTILACION 17     // El pin donde se conecta el Led
#define ESPACIOS 10

///////////Shared memory////////
int shared_fd_cant_autos = -1;
int *ptr_cant_autos = NULL;
#define SH_SIZE_CANT_AUTOS 1
int init_shared_resource();
///////////////////////////////////////

void *tareaA();
void *tareaB();
void *tareaC();
void *tareaD(void *arg);

int main(void)
{
    mqd_t mq;

    struct mq_attr attr;
    attr.mq_flags = 0;    // Se pueden pasar estados iniciales
    attr.mq_msgsize = 32; // El tamaño del mensaje en bytes
    attr.mq_maxmsg = 10;  // Cantidad maxima de mensajes en la cola
    attr.mq_curmsgs = 0;  // Cantidad inicial de datos encolados

    init_shared_resource();
    ptr_cant_autos = 0;
    pthread_t tarea1; //
    pthread_t tarea2; // Inicializo los hilos
    pthread_t tarea3; //
    pthread_t tarea4; //

    pthread_create(&tarea1, NULL, tareaA, NULL);  //
    pthread_create(&tarea2, NULL, tareaB, NULL);  //
    pthread_create(&tarea3, NULL, tareaC, NULL);  //
    pthread_create(&tarea4, NULL, tareaD, &attr); //
    pthread_join(tarea1, NULL);                   //
    pthread_join(tarea2, NULL);                   //
    pthread_join(tarea3, NULL);                   //
    pthread_join(tarea4, NULL);                   //

    return 0;
}

void *tareaA()
{
    mqd_t mq = mq_open("/mq0", O_WRONLY); // Permiso de escritura sobre la cola
    while (1)
    {
        int hayAuto = 0;
        if (*ptr_cant_autos < ESPACIOS)
        {
            printf("Luz verde");
            printf("Entro un auto ? \n(1) Para si \n(0) Para no");
            scanf("%d", &hayAuto);
            ptr_cant_autos += hayAuto;

            // Obtiene el tiempo actual en segundos desde el 1 de enero de 1970
            time_t tiempo_actual = time(NULL);

            // Convierte el tiempo actual en una estructura de tipo tm
            struct tm *fecha_hora = localtime(&tiempo_actual);

            int dia = fecha_hora->tm_mday;
            int mes = fecha_hora->tm_mon + 1;      // El mes se representa de 0 a 11, por eso se suma 1
            int anio = fecha_hora->tm_year + 1900; // Se suma 1900 al año
            char fecha[20];
            sprintf(fecha, "%02d/%02d/%04d", dia, mes, anio);

            int hora = fecha_hora->tm_hour;
            int minuto = fecha_hora->tm_min;

            char hora_minuto[10];
            sprintf(hora_minuto, "%02d:%02d", hora, minuto);

            char patente[20];
            printf("Ingrese su patente");
            scanf("%c", patente);

            //  waits while the parent is creating the queue
            sleep(1);
            //  writes to the write file descriptor
            sleep(2);
            fprintf(stdout, "CHILD: Writing to daddy ...\n"); // Se escribe en la cola
            mq_send(mq, fecha, strlen(fecha) + 1, 0);
            mq_send(mq, hora_minuto, strlen(hora_minuto) + 1, 0);
            mq_send(mq, patente, strlen(patente) + 1, 0);
            mq_send(mq, "A", 1 + 1, 0);
        }
        else
        {
            printf("Luz roja");
        }
    }
    mq_close(mq);
    return NULL;
}

void *tareaB()
{
    while (1)
    {
        /* code */
    }
    return NULL;
}

void *tareaC()
{
    while (1)
    {

        int salioAuto;
        printf("Salio algun auto ? \n(1) Para si \n(0) Para no");
        scanf("%d", &salioAuto);
        ptr_cant_autos -= salioAuto;
    }
    return NULL;
}

void *tareaD(void *arg)
{
    mqd_t mq = mq_open("/mq0", O_RDONLY | O_CREAT, 0644, arg);
    while (1)
    {

        char buff[60];
        int num_of_read_bytes = mq_receive(mq, buff, 60, NULL);
        fprintf(stdout, "Recibo: %s\n", buff);
    }
    mq_close(mq);
    mq_unlink("/mq0");
    return NULL;
}

// https://www.um.es/earlyadopters/actividades/a3/PCD_Activity3_Session1.pdf

int init_shared_resource()
{
    shared_fd_cant_autos = shm_open("/shm1", O_CREAT | O_RDWR, 0600);

    if (shared_fd_cant_autos < 0)
    {
        fprintf(stderr, "ERROR: Failed to create shared memory cant_autos: %s\n", strerror(errno));
        exit(1);
    }
    fprintf(stdout, "Shared memory is created with fd: %d\n", shared_fd_cant_autos);

    if (ftruncate(shared_fd_cant_autos, SH_SIZE_CANT_AUTOS * sizeof(int)) < 0)
    {
        fprintf(stderr, "ERROR: Truncation failed: %s\n", strerror(errno));
        return 1;
    }

    void *map_cant_autos = mmap(0, SH_SIZE_CANT_AUTOS, PROT_WRITE, MAP_SHARED, shared_fd_cant_autos, 0);

    if (map_cant_autos == MAP_FAILED)
    {
        fprintf(stderr, "ERROR: Mapping failed: %s\n", strerror(errno));
        return 1;
    }
    ptr_cant_autos = (int *)map_cant_autos;
}