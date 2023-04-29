#include <stdio.h> // Inicializo libreria para imprimir por pantalla
#include <time.h>  // Inicializo libreria para calcular los tiempos
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <wiringPi.h>      //
#include <linux/i2c-dev.h> //
#include <sys/ioctl.h>     //
#include <fcntl.h>         //


#define SH_SIZE_NAME 32
#define SH_SIZE_FECHA_HORA 32
#define SH_SIZE_TEMP_HORA 1


// shared memory object
int shared_fd_name = -1;
int shared_fd_tiempo_pulsado = -1;
int shared_fd_fecha_hora = -1;


char* ptr_fecha_hora = NULL;
char* ptr_name = NULL;
double* ptr_tiempo_pulsado = NULL;

double tiempoPulsado;

int main(void)
{
    int shm_fd_name = shm_open("/shm0", O_CREAT | O_RDWR, 0600);
    int shm_fd_tiempo_pulsado = shm_open("/shm1", O_CREAT | O_RDWR, 0600);
    int shm_fd_fecha_hora = shm_open("/shm2", O_CREAT | O_RDWR, 0600);
	
	fprintf(stdout, "Shared memory is created with fd: %d\n", shm_fd_name);
    fprintf(stdout, "Shared memory is created with fd: %d\n", shm_fd_tiempo_pulsado);
    fprintf(stdout, "Shared memory is created with fd: %d\n", shm_fd_fecha_hora);
	
	
    if (ftruncate(shared_fd_name, SH_SIZE * sizeof(char)) < 0) {
		fprintf(stderr, "ERROR: Truncation failed: %s\n", strerror(errno));
		return 1;
	}
    if (ftruncate(shared_fd_tiempo_pulsado, SH_SIZE * sizeof(double)) < 0) {
		fprintf(stderr, "ERROR: Truncation failed: %s\n", strerror(errno));
		return 1;
	}
    if (ftruncate(shared_fd_fecha_hora, SH_SIZE * sizeof(char)) < 0) {
		fprintf(stderr, "ERROR: Truncation failed: %s\n", strerror(errno));
		return 1;
	}
	
	void* map_name = mmap(0, SH_SIZE, PROT_WRITE, MAP_SHARED, shm_fd_name, 0);
    void* map_tiempo_pulsado = mmap(0, SH_SIZE, PROT_WRITE, MAP_SHARED, shm_fd_tiempo_pulsado, 0);
    void* map_fecha_hora = mmap(0, SH_SIZE, PROT_WRITE, MAP_SHARED, shm_fd_fecha_hora, 0);
	
	if (map_name == MAP_FAILED) {
		fprintf(stderr, "ERROR: Mapping failed: %s\n", strerror(errno));
		return 1;
	}
    if (map_tiempo_pulsado == MAP_FAILED) {
		fprintf(stderr, "ERROR: Mapping failed: %s\n", strerror(errno));
		return 1;
	}
    if (map_fecha_hora == MAP_FAILED) {
		fprintf(stderr, "ERROR: Mapping failed: %s\n", strerror(errno));
		return 1;
	}

    ptr_name = (char*)map_name;
    ptr_fecha_hora = (char*)map_fecha_hora;
    ptr_tiempo_pulsado = (double*) map_tiempo_pulsado;


    struct timeval ti, tf;

    pid_t ret = fork();

    if (ret)
    {
        
        double tiempo_pulsador;
       
        int presiono = 0;

        wiringPiSetupGpio();        // Establezco conexion con los pines
        pinMode(PULSADOR, INPUT);   // Declaro al pin 17 como pin de entrada
        pinMode(LED, OUTPUT);       // Declaro al pin 18 como pin de salida
        int pulsador_activo_fl = 0; // seteo un flag

        while (1)
        {
            pulsador_activo_fl = digitalRead(PULSADOR); // en 1 esta prendido, en 0 esta apagado
            if (pulsador_activo_fl && presiono == 0)
            {
                gettimeofday(&ti, NULL); // Instante inicial en que se presiona el pulsador
                presiono = 1;
                printf("Presionando Pulsador\n");
            }
            else
            {
                if (presiono)
                {
                    gettimeofday(&tf, NULL);                                                               // seteamos el tiempo final en que se termindo de presionar el pulsador
                    tiempo_pulsador = (tf.tv_sec - ti.tv_sec) * 1000 + (tf.tv_usec - ti.tv_usec) / 1000.0; // Obtenemos el tiempo pulsado
                    printf("Has tardado en pulsar : %g Seg\n", tiempo_pulsador / 1000);

                    tiempoPulsado = tiempo_pulsador / 1000;
                    presiono = 0;
                    *ptr_name = "BOTON PRESIONADO";
                    gettimeofday(ptr_fecha_hora, NULL);   
                    *ptr_tiempo_pulsado = tiempoPulsado;
                }
            }
        }
    }
    else
    {
         double tiempo_inicio_led_prender;
        for (;;)
        {
            tiempo_inicio_led_prender = 0;
            gettimeofday(&ti, NULL); // Instante inicio del led a prender
            
            fprintf(stdout, "La fecha es: %d.\n", *ptr_fecha_hora);
            fprintf(stdout, "El nombre del pulsador %d.\n", *ptr_name);
            fprintf(stdout, "El tiempo pulsado %d.\n", *ptr_tiempo_pulsado);

            while ((tiempo_inicio_led_prender < *ptr_tiempo_pulsado))
            {
                digitalWrite(LED, 0);
                gettimeofday(&tf, NULL);
                tiempo_inicio_led_prender = (tf.tv_sec - ti.tv_sec) * 1000 + (tf.tv_usec - ti.tv_usec) / 1000.0; // Obtenemos el tiempo que lleva el led prendido
            }
            digitalWrite(LED, 1); // Prendemos el LED
            printf("El periodo del LED es de : %g milisegundos\n", tiempo_inicio_led_prender);
            
        }
    }

    return 0;
}