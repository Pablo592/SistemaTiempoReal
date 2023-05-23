#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <mqueue.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>

#define MAX_LUGARES 10

// Estructura para almacenar los datos de un vehículo
typedef struct {
    char patente[8];
    char fecha[9];
    char hora[6];
    char puerta;
} Vehiculo;

// Variables compartidas
int vehiculos_dentro = 0;
sem_t sem_vehiculos;
sem_t sem_impresion;
mqd_t cola_mq;


// Función para generar una patente aleatoria
void generarPatente(char* patente) {
    static const char caracteres[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int i;
    for (i = 0; i < 7; i++) {
        patente[i] = caracteres[rand() % (sizeof(caracteres) - 1)];
    }
    patente[7] = '\0';
}

// Función para obtener la fecha actual en formato DD/MM/AA
void obtenerFechaActual(char* fecha) {
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    sprintf(fecha, "%02d/%02d/%02d", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year % 100);
}

// Función para obtener la hora actual en formato HH:MM
void obtenerHoraActual(char* hora) {
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    sprintf(hora, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
}

// Función para enviar un mensaje a la cola
void enviarMensajeCola(Vehiculo vehiculo) {
    char mensaje[256];
    sprintf(mensaje, "PATENTE: %s\nFECHA: %s\nHORA: %s\nINGRESO: %c \nCantidadOcupada: %d\n ",
            vehiculo.patente, vehiculo.fecha, vehiculo.hora, vehiculo.puerta,vehiculos_dentro);
    mq_send(cola_mq, mensaje, strlen(mensaje) + 1, 0);
}


// Función para el hilo de ingreso A
void* ingresoA(void* arg) {
    while (1) {
        sem_wait(&sem_vehiculos);
        if (vehiculos_dentro < MAX_LUGARES) {
            vehiculos_dentro++;
            sem_post(&sem_vehiculos);

            // Simular tiempo de autorización de ingreso
            sleep(1);

            printf("Autorización de ingreso para A: LED verde\n");

            // Generar datos del vehículo
            Vehiculo vehiculo;
            generarPatente(vehiculo.patente);
            obtenerFechaActual(vehiculo.fecha);
            obtenerHoraActual(vehiculo.hora);
            vehiculo.puerta = 'A';

            // Enviar mensaje a la cola
            enviarMensajeCola(vehiculo);
        } else {
            sem_post(&sem_vehiculos);
            printf("Plaza llena para A: LED rojo\n");
        }

        sleep(2);
    }
    return NULL;
}

// Función para el hilo de ingreso B
void* ingresoB(void* arg) {
    while (1) {
        sem_wait(&sem_vehiculos);
        if (vehiculos_dentro < MAX_LUGARES) {
            vehiculos_dentro++;
            sem_post(&sem_vehiculos);

            // Simular tiempo de autorización de ingreso
            sleep(1);

            printf("Autorización de ingreso para B: LED verde\n");

            // Generar datos del vehículo
            Vehiculo vehiculo;
            generarPatente(vehiculo.patente);
            obtenerFechaActual(vehiculo.fecha);
            obtenerHoraActual(vehiculo.hora);
            vehiculo.puerta = 'B';

            // Enviar mensaje a la cola
            enviarMensajeCola(vehiculo);
        } else {
            sem_post(&sem_vehiculos);
            printf("Plaza llena para B: LED rojo\n");
        }

        sleep(2);
    }
    return NULL;
}

// Función para el hilo de egreso C
void* egresoC(void* arg) {
    while (1) {
        
        sem_wait(&sem_vehiculos);
        if (vehiculos_dentro > 0) {
            vehiculos_dentro--;
            sem_post(&sem_vehiculos); //desbloqueo el semaforo
        } else {
            sem_post(&sem_vehiculos); //desbloqueo el semaforo
        }

        sleep(12);
    }
    return NULL;
}

// Función para el hilo D de impresión
void* impresionD(void* arg) {
    char buffer[256];
    unsigned int prio;

    while (1) {
        ssize_t msg_size = mq_receive(cola_mq, buffer, sizeof(buffer), &prio);
        if (msg_size >= 0) {
            sem_wait(&sem_impresion);   //bloqueo el semaforo

            printf("Imprimo el msj que recibo: %s\n", buffer);

            sem_post(&sem_impresion);   //desbloqueo el semaforo
        }
        
        sleep(5);
    }
    return NULL;
}

int main() {
    // Inicializar semáforos
    sem_init(&sem_vehiculos, 0, 1);
    sem_init(&sem_impresion, 0, 1);

    // Crear cola de mensajes
    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 256;
    attr.mq_flags = 0;
    cola_mq = mq_open("/cola_estacionamiento", O_CREAT | O_RDWR, 0666, &attr);

    // Crear hilos
    pthread_t hiloIngresoA, hiloIngresoB, hiloEgresoC, hiloImpresionD;
    pthread_create(&hiloIngresoA, NULL, ingresoA, NULL);
    pthread_create(&hiloIngresoB, NULL, ingresoB, NULL);
    pthread_create(&hiloEgresoC, NULL, egresoC, NULL);
    pthread_create(&hiloImpresionD, NULL, impresionD, NULL);

    // Esperar a que los hilos terminen
    pthread_join(hiloIngresoA, NULL);
    pthread_join(hiloIngresoB, NULL);
    pthread_join(hiloEgresoC, NULL);
    pthread_join(hiloImpresionD, NULL);

    // Cerrar cola de mensajes
    mq_close(cola_mq);
    mq_unlink("/cola_estacionamiento");

    // Destruir semáforos
    sem_destroy(&sem_vehiculos);
    sem_destroy(&sem_impresion);

    return 0;
}