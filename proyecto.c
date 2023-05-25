// #include <wiringPi.h>
// #include <softPwm.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
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
#include <sys/stat.h> //para obtener la configuracion del archivo

///////// PERIFERICOS ////
#define PWM_PIN 1 // El pin del serbo
#define ALARMA 17 // El pin del led/alarma


void *lectorDeArchivo();
void *monitoreaSensorHumedadTemperatura();
void *activaAlarma();
void *activaServomotor();
void *monitoreaCambiosArchivo();
void *monitoreaPulsador();

struct parameters
{
    double tempMax;
    double humMin;
    time_t horaRiego;
    time_t duracionMinutosRiego;
    time_t tiempoAnticipacionAlarma;
    time_t duracionMinutosAlarma;
};

struct sensor
{
    double temperatura;
    double humedad;
};

///////////MUTEX//////////////
// The pointer to shared mutex
pthread_mutex_t *mutex = NULL;
int mutex_shm_fd = -1;
void init_control_mechanism();
//////////////////////////////

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
bool comparaStr(char entrada[], char modelo[]);
time_t establecerFecha(int hora, int minutos);
time_t fechaActual();
void muevoSerbo(float grados);
void controloAlarma(bool estado);
char archivoNombre[20] = "riego.config";

int main(void)
{
    init_shared_resource();
    init_control_mechanism();

    pthread_t hilo1; //
    pthread_t hilo2; // Inicializo los hilos
    pthread_t hilo3; //
    pthread_t hilo4; //
    pthread_t hilo5; // Inicializo los hilos
    pthread_t hilo6; //

    pthread_create(&hilo1, NULL, lectorDeArchivo, NULL); //
    // pthread_create(&hilo2, NULL, monitoreaSensorHumedadTemperatura, NULL);
    pthread_create(&hilo3, NULL, activaAlarma, NULL);     //
    pthread_create(&hilo4, NULL, activaServomotor, NULL); //
    pthread_create(&hilo5, NULL, monitoreaCambiosArchivo, NULL);
    // pthread_create(&hilo6, NULL, monitoreaPulsador, NULL); //
    pthread_join(hilo1, NULL); //
    // pthread_join(hilo2, NULL);
    pthread_join(hilo3, NULL);
    pthread_join(hilo4, NULL);
    pthread_join(hilo5, NULL); //
    // pthread_join(hilo6, NULL);

    return 0;
}

void *lectorDeArchivo()
{
    while (1)
    {
        pthread_mutex_lock(mutex);
        FILE *file_pointer;                       // Declaro un puntero que va a ir apuntando al archivo fila por fila
        char str[150];                            // Declaro una variable que va a contener las filas del archivo
        file_pointer = fopen(archivoNombre, "r"); // Apunto el puntero al inicio del archivo
        int horaRiego = 0;
        int minutoRiego = 0;
        int duracionMinutosRiego = 0;
        int tiempoAnticipacionAlarma = 0;
        int duracionMinutosAlarma = 0;

        if (file_pointer == NULL)
        {
            printf("Error opening file!");
            exit(1);
        }

        while (fgets(str, 150, file_pointer) != NULL) // Voy recorriendo el archivo fila por fila
        {
            if (!starts_with(str, "#")) // Ignoro el instructivo del formato
            {
                char *ptr = strtok(str, ":"); // La posicion 0 del split(":")
                ptr = strtok(NULL, ":");      // La posicion 1 del split(":")

                if (starts_with(str, "TempMax")) // Asigno los valores a las variables correspondientes
                {
                    ptr_parameters->tempMax = atof(ptr);       // Parceo de char[] a int y guardo el numero en la estructura alojada en la memoria compartida
                    printf("'%f'\n", ptr_parameters->tempMax); // Imprimo el valor ajodado en la memoria compartida
                }
                else if (starts_with(str, "HumMin"))
                {
                    ptr_parameters->humMin = atof(ptr);
                    printf("'%f'\n", ptr_parameters->humMin);
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

        ptr_parameters->horaRiego = establecerFecha(horaRiego, minutoRiego);
        ptr_parameters->duracionMinutosRiego = establecerFecha(horaRiego, minutoRiego + duracionMinutosRiego);
        ptr_parameters->tiempoAnticipacionAlarma = establecerFecha(horaRiego, minutoRiego - tiempoAnticipacionAlarma);
        ptr_parameters->duracionMinutosAlarma = establecerFecha(horaRiego, minutoRiego - tiempoAnticipacionAlarma + (duracionMinutosAlarma == 0 ? tiempoAnticipacionAlarma : duracionMinutosAlarma));

        printf("Horario de riego: %s", ctime(&ptr_parameters->horaRiego));
        printf("Duracion de riego: %s", ctime(&ptr_parameters->duracionMinutosRiego));

        printf("Horario de alarma: %s", ctime(&ptr_parameters->tiempoAnticipacionAlarma));
        printf("Duracion de alarma: %s", ctime(&ptr_parameters->duracionMinutosAlarma));

        fclose(file_pointer); // Cierro el archivo
    }
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
    //wiringPiSetupGpio();  // Establezco conexion con los pines
    //pinMode(ALARMA, OUTPUT); // Declaro al pin 17 como pin de salida
    bool alarma = false;
    while (1)
    {
        time_t fechaAux = fechaActual();
        bool condicionHoraria = ((fechaAux >= ptr_parameters->tiempoAnticipacionAlarma) && (fechaAux < ptr_parameters->duracionMinutosAlarma));

        if (condicionHoraria)
        {

            printf("\nSuena el alarma\n");
            if (alarma == false)
            {
                alarma = true;
                printf("prendo el led");
                controloAlarma(alarma);
            }
        }
        else
        {
            if (alarma == true)
            {
                alarma = false;
                printf("apago el led");
                controloAlarma(alarma);
            }
        }
        sleep(1);
    }

    return NULL;
}

void *activaServomotor()
{

    float grados = 180;
    bool agua = false;

    /*
    wiringPiSetup();                // Inicializamos la biblioteca WiringPi
    pinMode(PWM_PIN, PWM_OUTPUT);   // Se establece que el pin sera de salida
    digitalWrite(PWM_PIN, 0);       // Se utiliza para escribir un valor digital (ALTO o BAJO) en el pin de la raspberry
    softPwmCreate(PWM_PIN, 0, 200); // crea una señal de modulación de ancho de pulso (PWM) impulsada por software en un pin GPIO específico.
                                    // softPwmCreate(int pin, int initialValue, int pwmRange);
    */

    while (1)
    {
        time_t fechaAux = fechaActual();
        bool condicionHoraria = ((fechaAux >= ptr_parameters->horaRiego) && (fechaAux < ptr_parameters->duracionMinutosRiego));
        bool condicionClimatica = ((ptr_sensor->temperatura > ptr_parameters->tempMax) && (ptr_sensor->humedad < ptr_parameters->humMin));
        bool pulsador = false;

        if (condicionHoraria || condicionClimatica || pulsador)
        {
            printf("\nsale el agua\n");
            if (agua == false)
            {
                agua = true;
                printf("muevo serbo");
                muevoSerbo(grados);
            }
        }

        if (!condicionHoraria)
        {
            if (condicionClimatica || pulsador)
            {
                printf("\nsigue saliendo el agua\n");
                break;
            }
            else
            {
                printf("\ncierro el agua\n");
                if (agua == true)
                {
                    agua = false;
                    printf("muevo serbo");
                    muevoSerbo(grados);
                }
            }
        }

        sleep(1);
        // printf("Temperatura: %lf\n", ptr_sensor->temperatura);
        // printf("Humedad: %lf\n", ptr_sensor->humedad);
    }
    return NULL;
}

void *monitoreaCambiosArchivo()
{
    char *filename = archivoNombre;
    struct stat file_stat;

    // Llama a la función stat() para obtener la información del archivo
    if (stat(filename, &file_stat) == -1)
    {
        printf("Error al obtener la información del archivo.\n");
    }

    // Extrae la fecha de modificación de la estructura stat
    time_t mod_time = file_stat.st_mtime;
    struct tm *time_info = localtime(&mod_time);

    // Formatea y muestra la fecha de modificación
    char fecha_anterior[50];
    strftime(fecha_anterior, sizeof(fecha_anterior), "%Y-%m-%d %H:%M:%S", time_info);
    // printf("La última modificación de %s fue el %s.\n", filename, fecha_anterior);

    while (1)
    {

        sleep(1);

        if (stat(filename, &file_stat) == -1)
        {
            printf("Error al obtener la información del archivo.\n");
        }

        // Extrae la fecha de modificación de la estructura stat
        time_t mod_times = file_stat.st_mtime;
        struct tm *time_info = localtime(&mod_times);

        // Formatea y muestra la fecha de modificación
        char fecha_actual[50];
        strftime(fecha_actual, sizeof(fecha_actual), "%Y-%m-%d %H:%M:%S", time_info);

        for (size_t i = 0; i < strlen(fecha_actual); i++)
        {
            if (fecha_anterior[i] != fecha_actual[i])
            {
                printf("Archivo modificado \n");
                pthread_mutex_unlock(mutex);

                for (size_t i = 0; i < strlen(fecha_actual); i++)
                {
                    fecha_anterior[i] = fecha_actual[i];
                }
                break;
            }
        }
    }

    return NULL;
}

void *monitoreaPulsador()
{
    return NULL;
}

// https://www.um.es/earlyadopters/actividades/a3/PCD_Activity3_Session1.pdf

time_t establecerFecha(int hora, int minutos)
{
    time_t currentTime;
    time(&currentTime);
    struct tm *localTime = localtime(&currentTime);
    localTime->tm_hour = hora;
    localTime->tm_min = minutos;
    localTime->tm_sec = 0;
    return mktime(localTime);
}

time_t fechaActual()
{
    time_t currentTime;
    time(&currentTime);
    struct tm *localTime = localtime(&currentTime);
    return mktime(localTime);
}

void muevoSerbo(float grados)
{
    /*
     float microsegundos;
     microsegundos = (((1 / 180) * grados) + 1) * 10; // Se hace la conversion de grados a milisegundos
     softPwmWrite(PWM_PIN, (microsegundos)); // Se establece cuanto debe durar la fase de la señal digital
    */
}

void controloAlarma(bool estado){
//    digitalWrite(ALARMA, estado); // Prendo el Led
}

bool starts_with(const char *str, const char *prefix)
{
    size_t prefix_len = strlen(prefix);
    if (strlen(str) < prefix_len)
    {
        return false;
    }
    return strncmp(str, prefix, prefix_len) == 0;
}

bool comparaStr(char entrada[], char modelo[])
{
    int ind = 0;

    while (entrada[ind] != '\0' && modelo[ind] != '\0' && entrada[ind] == modelo[ind])
        ind++;

    if (entrada[ind] != '\0' || modelo[ind] != '\0')
        return false;

    return true;
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

void init_control_mechanism()
{
    // Open the mutex shared memory
    mutex_shm_fd = shm_open("/mutex0", O_CREAT | O_RDWR, 0600);
    if (mutex_shm_fd < 0)
    {
        fprintf(stderr, "ERROR: Failed to create shared memory: %s\n", strerror(errno));
        exit(1);
    }
    // Allocate and truncate the mutex's shared memory region
    if (ftruncate(mutex_shm_fd, sizeof(pthread_mutex_t)) < 0)
    {
        fprintf(stderr, "ERROR: Truncation of mutex failed: %s\n",
                strerror(errno));
        exit(1);
    }
    // Map the mutex's shared memory
    void *map = mmap(0, sizeof(pthread_mutex_t),
                     PROT_READ | PROT_WRITE, MAP_SHARED, mutex_shm_fd, 0);
    if (map == MAP_FAILED)
    {
        fprintf(stderr, "ERROR: Mapping failed: %s\n",
                strerror(errno));
        exit(1);
    }
    mutex = (pthread_mutex_t *)map;
    // Initialize the mutex object
    int ret = -1;
    pthread_mutexattr_t attr;
    if ((ret = pthread_mutexattr_init(&attr)))
    {
        fprintf(stderr, "ERROR: Failed to init mutex attrs: %s\n",
                strerror(ret));
        exit(1);
    }
    if ((ret = pthread_mutexattr_setpshared(&attr,
                                            PTHREAD_PROCESS_SHARED)))
    {
        fprintf(stderr, "ERROR: Failed to set the mutex attr: %s\n",
                strerror(ret));
        exit(1);
    }
    if ((ret = pthread_mutex_init(mutex, &attr)))
    {
        fprintf(stderr, "ERROR: Initializing the mutex failed: %s\n",
                strerror(ret));
        exit(1);
    }
    if ((ret = pthread_mutexattr_destroy(&attr)))
    {
        fprintf(stderr, "ERROR: Failed to destroy mutex attrs : %s\n", strerror(ret));
        exit(1);
    }
}