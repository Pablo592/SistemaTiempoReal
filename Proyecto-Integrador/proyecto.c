// #include <wiringPi.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/time.h>
#include <errno.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>

void *lectorDeArchivo();
void *monitoreaSensorHumedadTemperatura();
void *activaAlarma();
void *activaServomotor();
void *monitoreaCambiosArchivo();
void *monitoreaPulsador();

struct parameters
{
    int tempMax;
    int humMin;
    int horaRiego;
    int minutoRiego;
    int duracionMinutosRiego;
    int tiempoAnticipacionAlarma;
    int duracionMinutosAlarma;
};

struct sensor
{
    double temperatura;
    double humedad;
};

///////////Shared memory////////
int shared_fd_parameters = -1;
int shared_fd_sensor = -1;
struct parameters *ptr_parameters = NULL;
struct sensor *ptr_sensor = NULL;
#define SH_SIZE_PARAMETERS 1
#define SH_SIZE_SENSOR 1
int init_shared_resource();
///////////////////////////////////////

#define AHT10_ADDRESS 0x38 // AHT10 I2C address

// AUX FUNCTIONS
bool starts_with(const char *str, const char *prefix);

int main(void)
{
    init_shared_resource();

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
    FILE *file_pointer;                        // Declaro un puntero que va a ir apuntando al archivo fila por fila
    char str[150];                             // Declaro una variable que va a contener las filas del archivo
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

            if (starts_with(str, "TempMax")) // Asigno los valores a las variables correspondientes
            {
                ptr_parameters->tempMax = atoi(ptr);       // Parceo de char[] a int y guardo el numero en la estructura alojada en la memoria compartida
                printf("'%d'\n", ptr_parameters->tempMax); // Imprimo el valor ajodado en la memoria compartida
            }
            else if (starts_with(str, "HumMin"))
            {
                ptr_parameters->humMin = atoi(ptr);
                printf("'%d'\n", ptr_parameters->humMin);
            }
            else if (starts_with(str, "HoraRiego"))
            {
                ptr_parameters->horaRiego = atoi(ptr);
                printf("'%d'\n", ptr_parameters->horaRiego);
            }
            else if (starts_with(str, "MinutoRiego"))
            {
                ptr_parameters->minutoRiego = atoi(ptr);
                printf("'%d'\n", ptr_parameters->minutoRiego);
            }
            else if (starts_with(str, "DuracionMinutosRiego"))
            {
                ptr_parameters->duracionMinutosRiego = atoi(ptr);
                printf("'%d'\n", ptr_parameters->duracionMinutosRiego);
            }
            else if (starts_with(str, "TiempoAnticipacionAlarma"))
            {
                ptr_parameters->tiempoAnticipacionAlarma = atoi(ptr);
                printf("'%d'\n", ptr_parameters->tiempoAnticipacionAlarma);
            }
            else if (starts_with(str, "DuracionMinutosAlarma"))
            {
                ptr_parameters->duracionMinutosAlarma = atoi(ptr);
                printf("'%d'\n", ptr_parameters->duracionMinutosAlarma);
            }
        }
    }

    fclose(file_pointer); // Cierro el archivo
    return NULL;
}

void *monitoreaSensorHumedadTemperatura()
{
    /*  wiringPiSetupGpio();
      int file;
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
  */
    // Send command to measure temperature and humidity
    char command[3] = {0xAC, 0x33, 0x00}; // Busco en memoria la humedad y temperatura detectada por el sensor

    double cur_temp, ctmp;
    while (1) // El sensor recopila datos sin parar
    {
        /*    write(file, command, 3);
            usleep(50000); // Se espera un tiempo a que las mediciones puedan completarse (50ms)
            char data[6];
            read(file, data, 6);

            cur_temp = (((data[3] & 0x0F) << 16) | (data[4] << 8) | data[5]);
            cur_temp = ((cur_temp * 200) / 1048576) - 50;
            printf("Temperature: %2.2f\n", cur_temp); // Se imprime la temperatura monitoreada por consola
            ctmp = ((data[1] << 16) | (data[2] << 8) | data[3]) >> 4;
            ctmp = ctmp * 100 / 1048576;
            printf("Humidity: %1.f %\n", ctmp); // Se imprime la humedad monitoreada por consola
    */
        cur_temp += 0.1;
        ctmp += 0.1;
        sleep(1);
        ptr_sensor->temperatura = cur_temp;
        ptr_sensor->humedad = ctmp;
    }

    //   close(file);

    return NULL;
}

void *activaAlarma()
{
    return NULL;
}

void *activaServomotor()
{
    while (1)
    {
        sleep(1);
        printf("Temperatura: %lf\n", ptr_sensor->temperatura);
        printf("Humedad: %lf\n", ptr_sensor->humedad);
    }
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

int init_shared_resource()
{

    shared_fd_parameters = shm_open("/shm1", O_CREAT | O_RDWR, 0600);
    shared_fd_sensor = shm_open("/shm1", O_CREAT | O_RDWR, 0600);

    if (shared_fd_parameters < 0)
    {
        fprintf(stderr, "ERROR: Failed to create shared memory PARAMETERS: %s\n", strerror(errno));
        exit(1);
    }
    if (shared_fd_sensor < 0)
    {
        fprintf(stderr, "ERROR: Failed to create shared memory SENSOR: %s\n", strerror(errno));
        exit(1);
    }
    fprintf(stdout, "Shared memory is created with fd: %d\n", shared_fd_parameters);
    fprintf(stdout, "Shared memory is created with fd: %d\n", shared_fd_sensor);

    if (ftruncate(shared_fd_parameters, SH_SIZE_PARAMETERS * sizeof(struct parameters)) < 0)
    {
        fprintf(stderr, "ERROR: Truncation failed: %s\n", strerror(errno));
        return 1;
    }
    if (ftruncate(shared_fd_sensor, SH_SIZE_SENSOR * sizeof(struct sensor)) < 0)
    {
        fprintf(stderr, "ERROR: Truncation failed: %s\n", strerror(errno));
        return 1;
    }

    void *map_parameters = mmap(0, SH_SIZE_PARAMETERS, PROT_WRITE, MAP_SHARED, shared_fd_parameters, 0);
    void *map_sensor = mmap(0, SH_SIZE_SENSOR, PROT_WRITE, MAP_SHARED, shared_fd_sensor, 0);

    if (map_parameters == MAP_FAILED)
    {
        fprintf(stderr, "ERROR: Mapping failed: %s\n", strerror(errno));
        return 1;
    }
    if (map_sensor == MAP_FAILED)
    {
        fprintf(stderr, "ERROR: Mapping failed: %s\n", strerror(errno));
        return 1;
    }

    ptr_parameters = (struct parameters *)map_parameters;
    ptr_sensor = (struct sensor *)map_sensor;
}