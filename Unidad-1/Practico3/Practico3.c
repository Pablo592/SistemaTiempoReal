#include <wiringPi.h>
#include <softPwm.h>
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
#include <sys/stat.h>

///////// PERIFERICOS ////
#define PWM_PIN 26          // El pin del serbo
#define ALARMA 5            // LED ALARMA ACTIVO = AGUA SALE, DESACTIVADO = NO SALE AGUA
#define PULSADOR 18       
#define AHT10_ADDRESS 0x38 // Sensor Humedad y Temperatura

// Variables de riego extraidas del archivos de riego
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
//READ/WRITE de variables de Archivo,Pulsador, Humedad y Temperatura
pthread_mutex_t *mutex = NULL;
pthread_mutex_t *mutex_pulsador = NULL;
pthread_mutex_t *mutex_temp_hum = NULL;
int mutex_shm_fd = -1;
int mutex_shm_fd_pulsador = -1;
int mutex_shm_fd_temp_hum = -1;
//Config de los Mutexs
void init_control_mechanism(); 

///////////Shared memory////////
int shared_fd_parameters = -1;
int shared_fd_sensor = -1;
int shared_fd_pulsador = -1;
struct parameters *ptr_parameters = NULL;
struct sensor *ptr_sensor = NULL;
bool *ptr_pulsador = false;
#define SH_SIZE_PARAMETERS 1
#define SH_SIZE_SENSOR 1
#define SH_SIZE_PULSADOR 1
///Config de Shared Memory
int init_shared_resource();

// Funciones de cada Hilo
void *lectorDeArchivo();
void *monitoreaSensorHumedadTemperatura();
void *activaAlarma();
void *activaServomotor();
void *monitoreaCambiosArchivo();
void *monitoreaPulsador();

// AUX FUNCTIONS
bool starts_with(const char *str, const char *prefix);
bool comparaStr(char entrada[], char modelo[]);
time_t establecerFecha(int hora, int minutos);
time_t fechaActual();
void abroValvula();
void cierroValvula();
void controloAlarma(bool estado);

// Variables Globales
double frecuencia_Actualizacion_Temp_And_Hume_En_Seg = 1;
char archivoNombre[20] = "riego.config";
int pulso = false;

int main(void)
{
    init_shared_resource();
    init_control_mechanism();
    wiringPiSetupGpio();    // Establezco conexion de los pines de la Raspberry
    pinMode(ALARMA, OUTPUT);
    pinMode(PULSADOR, INPUT);
    
    //Inicializo los hilos
    pthread_t hilo1;
    pthread_t hilo2;
    pthread_t hilo3;
    pthread_t hilo4;
    pthread_t hilo5;
    pthread_t hilo6;
    //Asignamos las responsabilidades a cada Hilo
    pthread_create(&hilo1, NULL, lectorDeArchivo, NULL);
    pthread_create(&hilo2, NULL, monitoreaSensorHumedadTemperatura, NULL);
    pthread_create(&hilo3, NULL, activaAlarma, NULL);
    pthread_create(&hilo4, NULL, activaServomotor, NULL);
    pthread_create(&hilo5, NULL, monitoreaCambiosArchivo, NULL);
    pthread_create(&hilo6, NULL, monitoreaPulsador, NULL);
    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);
    pthread_join(hilo3, NULL);
    pthread_join(hilo4, NULL);
    pthread_join(hilo5, NULL);
    pthread_join(hilo6, NULL);

    return 0;
}

void *lectorDeArchivo()
{
    while (1)
    {
        pthread_mutex_lock(mutex);                //No leer el archivo a menos que se haya actualizado el achivo de Configuracion"
        FILE *file_pointer;                       // Ptr apunta fila por fila en el archivo
        char str[150];                            // Tamaño maximo de la fila
        file_pointer = fopen(archivoNombre, "r"); // Apunto el Ptr al inicio del archivo
        
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

        while (fgets(str, 150, file_pointer) != NULL)   //Recorriendo el archivo fila x fila
        {
            if (!starts_with(str, "#"))                 // Ignoro el instructivo del formato
            {
                char *ptr = strtok(str, ":"); // La posicion 0 del split(":")
                ptr = strtok(NULL, ":");      // La posicion 1 del split(":")

                if (starts_with(str, "TempMax"))
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
                else if (starts_with(str, "Frecuencia_Actualizacion_Temp_And_Hume_En_Seg"))
                {
                    frecuencia_Actualizacion_Temp_And_Hume_En_Seg = atof(ptr);
                    printf("'%f'\n", frecuencia_Actualizacion_Temp_And_Hume_En_Seg);
                }
            }
        }

        //Formulas para calcular los intervalos de calendario
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
    int file;
    char *filename = "/dev/i2c-1";                          // I2C Bus Device File
    if ((file = open(filename, O_RDWR)) < 0)                // Verifico Conexion Interfaz I2C
    {
        printf("Failed to open I2C bus %s\n", filename);
        exit(1);
    }

    if (ioctl(file, I2C_SLAVE, AHT10_ADDRESS) < 0)          // Verifico Conexion Sensor AHT10
    {
        printf("Failed to select AHT10 sensor\n");
        exit(1);
    }

    char command[3] = {0xAC, 0x33, 0x00};                   // Posiciones de memoria asignada para almacenar la humedad y temperatura

    double cur_temp, cur_hum, tiempo;
    struct timeval ti, tf;
    gettimeofday(&ti, NULL);

    // El sensor recopila datos sin parar
    while (1) 
    {
        tiempo = (tf.tv_sec - ti.tv_sec) * 1000 + (tf.tv_usec - ti.tv_usec) / 1000.0;
        tiempo = tiempo / 1000;                             // Tiempo en seg recien cuando se lo divide por 1000
        if (tiempo <= frecuencia_Actualizacion_Temp_And_Hume_En_Seg)
            gettimeofday(&tf, NULL);
        else
        {
            gettimeofday(&ti, NULL);
            write(file, command, 3);
            usleep(50000);                                  // Espera activa en que las mediciones puedan completarse (50ms)
            char data[6];
            read(file, data, 6);

            //Lectura Temperatura
            cur_temp = (((data[3] & 0x0F) << 16) | (data[4] << 8) | data[5]);
            cur_temp = ((cur_temp * 200) / 1048576) - 50;
            ptr_sensor->temperatura = cur_temp;
                                                                    
            //Lectura Humedad
            cur_hum = ((data[1] << 16) | (data[2] << 8) | data[3]) >> 4;
            cur_hum = cur_hum * 100 / 1048576;
            ptr_sensor->humedad = cur_hum;
        }
        // Se permite el acceso a las variables de Hum y Temp a los demas hilos
        pthread_mutex_unlock(mutex_temp_hum); 
    }
    close(file);

    return NULL;
}


//Alarma suena un poco antes del horario programado
void *activaAlarma()
{
    bool alarma = false;
    while (1)
    {
        time_t fechaAux = fechaActual();
        bool condicionHoraria = ((fechaAux >= ptr_parameters->tiempoAnticipacionAlarma) && (fechaAux < ptr_parameters->duracionMinutosAlarma));
        if (condicionHoraria)
        {
            if (alarma == false)
            {   
                alarma = true;
                controloAlarma(alarma);
            }
        }
        else
        {
            if (alarma == true)
            {
                alarma = false;
                controloAlarma(alarma);
            }
        }
        sleep(1);
    }
    return NULL;
}

// Se activa:
//      + Un rato antes del horario programado y durante el mismo.
//      + Humedad = Por debajo del Umbral aceptable.
//      + Temperatura = Por ensima del umbral permitido.
//      + Pulsador = Apretado
void *activaServomotor()
{
    //Config Servo
    wiringPiSetup();
    pinMode(PWM_PIN, PWM_OUTPUT);

    bool agua = false;   
    while (1)
    {
        // pthread_mutex_lock(mutex_pulsador);
        time_t fechaAux = fechaActual();
        pthread_mutex_lock(mutex_temp_hum);

        printf("Temperatura Actual: %2.2f\n", ptr_sensor->temperatura);
        printf("Temperatura Maxima Permitida: %2.2f\n", ptr_parameters->tempMax);
        printf("Humedad Actual: %2.2f\n", ptr_sensor->humedad);
        printf("Humedad Minima permitida: %2.2f\n", ptr_parameters->humMin);

        bool condicionHoraria = ((fechaAux >= ptr_parameters->horaRiego) && (fechaAux < ptr_parameters->duracionMinutosRiego));
        bool condicionClimatica = ((ptr_sensor->temperatura > ptr_parameters->tempMax) || (ptr_sensor->humedad < ptr_parameters->humMin));

        if ((condicionHoraria || condicionClimatica || *ptr_pulsador))
        {
            if (agua == false)
            {
                agua = true;                
                abroValvula();
                controloAlarma(agua);   // Prendo Led
            }
        }

        if (!condicionHoraria)
        {
            if ((condicionClimatica == false) && (*ptr_pulsador == false))
            {
                if (agua == true)
                {
                    agua = false;
                    cierroValvula();
                    controloAlarma(agua);   // Apago Led
                }
            }
        }
        sleep(1);
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
                pthread_mutex_unlock(mutex);                        // Permito a otros hilos acceder a las leer el archivo

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
    *ptr_pulsador = false;
    int aux_pulsador = false;

    while (1)
    {
        pulso = digitalRead(PULSADOR);  // 1 =  Pulsado, 0 = Suelto

        if (pulso != 0)
        {
            while (0 != pulso)
            {
                pulso = digitalRead(PULSADOR);
            }
            
            aux_pulsador = !aux_pulsador;
            *ptr_pulsador = aux_pulsador;
            
            if (*ptr_pulsador)
                printf("\n Abroooooooooo el agua\n");
            else
                printf("\n Cierrroooooooo el agua\n");
        }
        
        // pthread_mutex_unlock(mutex_pulsador);
    }

    return NULL;
}

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

void abroValvula()
{
    printf("abroValvula-->    Seerbo \n");
    for (int intensity = 0; intensity < 1024; ++intensity)
    {
        pwmWrite(PWM_PIN, intensity);
        delay(1);
    }
}

void cierroValvula()
{
    printf("cierroValvula-->    Seerbo \n");
    for (int intensity = 1023; intensity >= 0; --intensity)
    {
        pwmWrite(PWM_PIN, intensity);
        delay(1);
    }
}

void controloAlarma(bool estado)
{
    digitalWrite(ALARMA, estado); // Prendo el Led
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

// Compara caracteres de la misma posicion de las cadenas de entrada omitiendo caracter de fin de linea = (\0)
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
    shared_fd_sensor = shm_open("/shm2", O_CREAT | O_RDWR, 0600);
    shared_fd_pulsador = shm_open("/shm3", O_CREAT | O_RDWR, 0600);

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
    if (shared_fd_pulsador < 0)
    {
        fprintf(stderr, "ERROR: Failed to create shared memory SENSOR: %s\n", strerror(errno));
        exit(1);
    }
    fprintf(stdout, "Shared memory is created with fd: %d\n", shared_fd_parameters);
    fprintf(stdout, "Shared memory is created with fd: %d\n", shared_fd_sensor);
    fprintf(stdout, "Shared memory is created with fd: %d\n", shared_fd_pulsador);

    // Calculo de tamaño exacto de 1 unica estructura en memoria
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
    if (ftruncate(shared_fd_pulsador, SH_SIZE_PULSADOR * sizeof(bool)) < 0)
    {
        fprintf(stderr, "ERROR: Truncation failed: %s\n", strerror(errno));
        return 1;
    }

    // Mapeo de los objetos en RAM
    void *map_parameters = mmap(0, SH_SIZE_PARAMETERS, PROT_WRITE, MAP_SHARED, shared_fd_parameters, 0);
    void *map_sensor = mmap(0, SH_SIZE_SENSOR, PROT_WRITE, MAP_SHARED, shared_fd_sensor, 0);
    void *map_pulsador = mmap(0, SH_SIZE_SENSOR, PROT_WRITE, MAP_SHARED, shared_fd_pulsador, 0);

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
    if (map_pulsador == MAP_FAILED)
    {
        fprintf(stderr, "ERROR: Mapping failed: %s\n", strerror(errno));
        return 1;
    }

    // redirecciona los ptrs a sus correspondientes estructuras
    ptr_parameters = (struct parameters *)map_parameters;
    ptr_sensor = (struct sensor *)map_sensor;
    ptr_pulsador = (bool *)map_pulsador;
}

void init_control_mechanism()
{
    // Open the mutex shared memory
    mutex_shm_fd = shm_open("/mutex0", O_CREAT | O_RDWR, 0600);
    mutex_shm_fd_pulsador = shm_open("/mutex1", O_CREAT | O_RDWR, 0600);
    mutex_shm_fd_temp_hum = shm_open("/mutex2", O_CREAT | O_RDWR, 0600);

    if (mutex_shm_fd < 0)
    {
        fprintf(stderr, "ERROR: Failed to create shared memory mutex_shm_fd: %s\n", strerror(errno));
        exit(1);
    }

    if (mutex_shm_fd_pulsador < 0)
    {
        fprintf(stderr, "ERROR: Failed to create shared memory mutex_shm_fd_pulsador: %s\n", strerror(errno));
        exit(1);
    }

    if (mutex_shm_fd_temp_hum < 0)
    {
        fprintf(stderr, "ERROR: Failed to create shared memory mutex_shm_fd_temp_hum: %s\n", strerror(errno));
        exit(1);
    }

    // Allocate and truncate the mutex's shared memory region
    if (ftruncate(mutex_shm_fd, sizeof(pthread_mutex_t)) < 0)
    {
        fprintf(stderr, "ERROR: Truncation of mutex failed: %s\n",
                strerror(errno));
        exit(1);
    }
    if (ftruncate(mutex_shm_fd_pulsador, sizeof(pthread_mutex_t)) < 0)
    {
        fprintf(stderr, "ERROR: Truncation of mutex failed: %s\n",
                strerror(errno));
        exit(1);
    }

    if (ftruncate(mutex_shm_fd_temp_hum, sizeof(pthread_mutex_t)) < 0)
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

    void *map_pulsador = mmap(0, sizeof(pthread_mutex_t),
                              PROT_READ | PROT_WRITE, MAP_SHARED, mutex_shm_fd_pulsador, 0);
    if (map_pulsador == MAP_FAILED)
    {
        fprintf(stderr, "ERROR: Mapping failed: %s\n",
                strerror(errno));
        exit(1);
    }

    void *map_temp_hum = mmap(0, sizeof(pthread_mutex_t),
                              PROT_READ | PROT_WRITE, MAP_SHARED, mutex_shm_fd_temp_hum, 0);
    if (map_temp_hum == MAP_FAILED)
    {
        fprintf(stderr, "ERROR: Mapping failed: %s\n",
                strerror(errno));
        exit(1);
    }

    mutex = (pthread_mutex_t *)map;
    mutex_pulsador = (pthread_mutex_t *)map_pulsador;
    mutex_temp_hum = (pthread_mutex_t *)map_temp_hum;

    // Initialize the mutex object
    int ret_pulsador = -1;
    int ret = -1;
    int ret_temp_hum = -1;
    pthread_mutexattr_t attr;
    pthread_mutexattr_t attr_pulsador;
    pthread_mutexattr_t attr_temp_hum;

    if ((ret = pthread_mutexattr_init(&attr)))
    {
        fprintf(stderr, "ERROR: Failed to init mutex attrs: %s\n", strerror(ret));
        exit(1);
    }

    if ((ret_pulsador = pthread_mutexattr_init(&attr_pulsador)))
    {
        fprintf(stderr, "ERROR: Failed to init mutex attrs: %s\n", strerror(ret_pulsador));
        exit(1);
    }

    if ((ret_temp_hum = pthread_mutexattr_init(&attr_temp_hum)))
    {
        fprintf(stderr, "ERROR: Failed to init mutex attrs: %s\n", strerror(ret_temp_hum));
        exit(1);
    }

    if ((ret = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED)))
    {
        fprintf(stderr, "ERROR: Failed to set the mutex attr: %s\n", strerror(ret));
        exit(1);
    }

    if ((ret_pulsador = pthread_mutexattr_setpshared(&attr_pulsador, PTHREAD_PROCESS_SHARED)))
    {
        fprintf(stderr, "ERROR: Failed to set the mutex attr: %s\n", strerror(ret_pulsador));
        exit(1);
    }

    if ((ret_temp_hum = pthread_mutexattr_setpshared(&attr_temp_hum, PTHREAD_PROCESS_SHARED)))
    {
        fprintf(stderr, "ERROR: Failed to set the mutex attr: %s\n", strerror(ret_temp_hum));
        exit(1);
    }

    if ((ret = pthread_mutex_init(mutex, &attr)))
    {
        fprintf(stderr, "ERROR: Initializing the mutex failed: %s\n", strerror(ret));
        exit(1);
    }

    if ((ret_pulsador = pthread_mutex_init(mutex_pulsador, &attr_pulsador)))
    {
        fprintf(stderr, "ERROR: Initializing the mutex failed: %s\n", strerror(ret_pulsador));
        exit(1);
    }

    if ((ret_temp_hum = pthread_mutex_init(mutex_temp_hum, &attr_temp_hum)))
    {
        fprintf(stderr, "ERROR: Initializing the mutex failed: %s\n", strerror(ret_temp_hum));
        exit(1);
    }

    if ((ret = pthread_mutexattr_destroy(&attr)))
    {
        fprintf(stderr, "ERROR: Failed to destroy mutex attrs : %s\n", strerror(ret));
        exit(1);
    }

    if ((ret_pulsador = pthread_mutexattr_destroy(&attr_pulsador)))
    {
        fprintf(stderr, "ERROR: Failed to destroy mutex attrs : %s\n", strerror(ret_pulsador));
        exit(1);
    }

    if ((ret_temp_hum = pthread_mutexattr_destroy(&attr_temp_hum)))
    {
        fprintf(stderr, "ERROR: Failed to destroy mutex attrs : %s\n", strerror(ret_temp_hum));
        exit(1);
    }
}