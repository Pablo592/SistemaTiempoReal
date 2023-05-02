#include <stdio.h> // Inicializo libreria para imprimir por pantalla
#include <time.h>  // Inicializo libreria para calcular los tiempos
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

void *lectorDeArchivo();
void *monitoreaSensorHumedadTemperatura();
void *activaAlarma();
void *activaServomotor();
void *monitoreaCambiosArchivo();
void *monitoreaPulsador();

int tempMax;
int humMin;
int horaRiego;
int minutoRiego;
int duracionMinutosRiego;
int tiempoAnticipacionAlarma;
int duracionMinutosAlarma;

// AUX FUNCTIONS
bool starts_with(const char *str, const char *prefix);

int main(void)
{
    pthread_t hilo1; //
    pthread_t hilo2; // Inicializo los hilos
    pthread_t hilo3; //
    pthread_t hilo4; //
    pthread_t hilo5; // Inicializo los hilos
    pthread_t hilo6; //

    pthread_create(&hilo1, NULL, lectorDeArchivo, NULL); //
    pthread_create(&hilo2, NULL, monitoreaSensorHumedadTemperatura, NULL);
    pthread_create(&hilo3, NULL, activaAlarma, NULL);     //
    pthread_create(&hilo4, NULL, activaServomotor, NULL); //
    pthread_create(&hilo5, NULL, monitoreaCambiosArchivo, NULL);
    pthread_create(&hilo6, NULL, monitoreaPulsador, NULL); //
    pthread_join(hilo1, NULL);                             //
    pthread_join(hilo2, NULL);                             // Espero a que el hilo finalice su ejecucion
    pthread_join(hilo3, NULL);                             //
    pthread_join(hilo4, NULL);                             //
    pthread_join(hilo5, NULL);                             // Espero a que el hilo finalice su ejecucion
    pthread_join(hilo6, NULL);                             //

    return 0;
}

void *lectorDeArchivo()
{
    FILE *file_pointer; //Declaro un puntero que va a ir apuntando al archivo fila por fila
    char str[150];  // Declaro una variable que va a contener las filas del archivo
    file_pointer = fopen("riego.config", "r"); // Apunto el puntero al inicio del archivo

    if (file_pointer == NULL)
    {
        printf("Error opening file!");
        exit(1);
    }

    while (fgets(str, 150, file_pointer) != NULL) // Voy recorriendo el archivo fila por fila
    {
        if (starts_with(str, "FIN_FILE")) // Cuando llego al final del archivo dejo de leer
            break;

        if (!starts_with(str, "#")) // Ignoro el instructivo del formato
        {
            char *ptr = strtok(str, ":"); // La posicion 0 del split(":")
            ptr = strtok(NULL, ":");      // La posicion 1 del split(":")

            if (starts_with(str, "TempMax"))    // Asigno los valores a las variables correspondientes
            {
                tempMax = atoi(ptr);        // Parceo de char[] a int
                printf("'%d'\n", tempMax);
            }
            else if (starts_with(str, "HumMin"))
            {
                humMin = atoi(ptr);
                printf("'%d'\n", humMin);
            }
            else if (starts_with(str, "HoraRiego"))
            {
                horaRiego = atoi(ptr);
                printf("'%d'\n", horaRiego);
            }
            else if (starts_with(str, "MinutoRiego"))
            {
                minutoRiego = atoi(ptr);
                printf("'%d'\n", minutoRiego);
            }
            else if (starts_with(str, "DuracionMinutosRiego"))
            {
                duracionMinutosRiego = atoi(ptr);
                printf("'%d'\n", duracionMinutosRiego);
            }
            else if (starts_with(str, "TiempoAnticipacionAlarma"))
            {
                tiempoAnticipacionAlarma = atoi(ptr);
                printf("'%d'\n", tiempoAnticipacionAlarma);
            }
            else if (starts_with(str, "DuracionMinutosAlarma"))
            {
                duracionMinutosAlarma = atoi(ptr);
                printf("'%d'\n", duracionMinutosAlarma);
            }
        }
    }

    fclose(file_pointer);   // Cierro el archivo
    return NULL;
}

void *monitoreaSensorHumedadTemperatura()
{
    return NULL;
}

void *activaAlarma()
{
    return NULL;
}

void *activaServomotor()
{
    return NULL;
}

void *monitoreaCambiosArchivo()
{
    return NULL;
}

void *monitoreaPulsador()
{
    return NULL;
}

// https://www.um.es/earlyadopters/actividades/a3/PCD_Activity3_Session1.pdf

bool starts_with(const char *str, const char *prefix)
{
    size_t prefix_len = strlen(prefix);
    if (strlen(str) < prefix_len)
    {
        return false;
    }
    return strncmp(str, prefix, prefix_len) == 0;
}