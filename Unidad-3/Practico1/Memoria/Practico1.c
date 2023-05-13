#include <stdio.h> // Inicializo libreria para imprimir por pantalla
#include <time.h>  // Inicializo libreria para calcular los tiempos
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
//#include <wiringPi.h>      //
#include <linux/i2c-dev.h> //
#include <sys/ioctl.h>     //
#include <fcntl.h>         //
#include <stdint.h>
#include <sys/mman.h>
//#include <sys/wait.h>

#define SH_SIZE_NAME 32
#define SH_SIZE_FECHA_HORA 32
#define SH_SIZE_TIEMPO_PULSADO 1
// shared memory object
int shared_fd_name = -1;
int shared_fd_tiempo_pulsado = -1;
int shared_fd_fecha_hora = -1;

void init_shared_resource() {

    shared_fd_name = shm_open("/shm0", O_CREAT | O_RDWR, 0600);
    shared_fd_tiempo_pulsado = shm_open("/shm1", O_CREAT | O_RDWR, 0600);
    shared_fd_fecha_hora = shm_open("/shm2", O_CREAT | O_RDWR, 0600);
	
	if (shared_fd_name < 0) {
		fprintf(stderr, "ERROR: Failed to create shared memory NAME: %s\n", strerror(errno));
		exit(1);
	}
    if (shared_fd_tiempo_pulsado < 0) {
		fprintf(stderr, "ERROR: Failed to create shared memory TIEMPO_PULSADO: %s\n", strerror(errno));
		exit(1);
	}
    if (shared_fd_fecha_hora < 0) {
		fprintf(stderr, "ERROR: Failed to create shared memory FECHA_HORA: %s\n", strerror(errno));
		exit(1);
	}
	
	fprintf(stdout, "Shared memory is created with fd: %d\n", shared_fd_name);
    fprintf(stdout, "Shared memory is created with fd: %d\n", shared_fd_tiempo_pulsado);
    fprintf(stdout, "Shared memory is created with fd: %d\n", shared_fd_fecha_hora);
	
}




char* ptr_fecha_hora = NULL;
char* ptr_name = NULL;
double* ptr_tiempo_pulsado = NULL;

double tiempoPulsado;



int main(void)
{
   
	init_shared_resource();

	
    if (ftruncate(shared_fd_name, SH_SIZE_NAME * sizeof(char)) < 0) {
		fprintf(stderr, "ERROR: Truncation failed: %s\n", strerror(errno));
		return 1;
	}
    if (ftruncate(shared_fd_tiempo_pulsado, SH_SIZE_TIEMPO_PULSADO * sizeof(double)) < 0) {
		fprintf(stderr, "ERROR: Truncation failed: %s\n", strerror(errno));
		return 1;
	}
    if (ftruncate(shared_fd_fecha_hora, SH_SIZE_FECHA_HORA * sizeof(int)) < 0) {
		fprintf(stderr, "ERROR: Truncation failed: %s\n", strerror(errno));
		return 1;
	}
	
	void* map_name = mmap(0, SH_SIZE_NAME, PROT_WRITE, MAP_SHARED, shared_fd_name, 0);
    void* map_tiempo_pulsado = mmap(0, SH_SIZE_TIEMPO_PULSADO, PROT_WRITE, MAP_SHARED, shared_fd_tiempo_pulsado, 0);
    void* map_fecha_hora = mmap(0, SH_SIZE_FECHA_HORA, PROT_WRITE, MAP_SHARED, shared_fd_fecha_hora, 0);
	
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


     int pulsador_activo_fl;
    //printf("Introduzca 1 = PULSADO, 0 <> NO PULSADO: ");
    //scanf("%d", &pulsador_activo_fl);

    if (ret)
    {
        
        double tiempo_pulsador;
       
        int presiono = 0;
        /*
        wiringPiSetupGpio();        // Establezco conexion con los pines
        pinMode(PULSADOR, INPUT);   // Declaro al pin 17 como pin de entrada
        pinMode(LED, OUTPUT);       // Declaro al pin 18 como pin de salida
        
        
        */
       int pulsador_activo_fl = 0; // seteo un flag
        while (1)
        {
            printf("Introduzca 1 = PULSADO, 0 <> NO PULSADO:\n");
            scanf("%d", &pulsador_activo_fl);
            /*pulsador_activo_fl = digitalRead(PULSADOR);*/ // en 1 esta prendido, en 0 esta apagado
            if (pulsador_activo_fl && presiono == 0)
            {
                gettimeofday(&ti, NULL); // Instante inicial en que se presiona el pulsador
                presiono = 1;
                printf("Presionando Pulsador\n");
                pulsador_activo_fl = 1;
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
                    
                    time_t t = time(NULL);
                    struct tm tm = *localtime(&t);
                    //printf("%02d:%02d:%02d  %d-%02d-%02d\n",tm.tm_hour, tm.tm_min, tm.tm_sec, tm.tm_mday, tm.tm_mon + 1,tm.tm_year + 1900);
                    

                    *ptr_tiempo_pulsado = tiempoPulsado;
                   // char result[100]; // 
                   // *ptr_fecha_hora = sprintf(result,"%02d:%02d:%02d  %d-%02d-%02d",tm.tm_hour, tm.tm_min, tm.tm_sec, tm.tm_mday, tm.tm_mon + 1,tm.tm_year + 1900);
                   // ptr_name = "BOTON 1";
                }
            }
        }
    }
    else
    {
         double tiempo_inicio_led_prender;
         //printf("%lf",*ptr_tiempo_pulsado);

        for (;;)
        {
            tiempo_inicio_led_prender = 0;
            gettimeofday(&ti, NULL); // Instante inicio del led a prender
            //if(*ptr_tiempo_pulsado >0)
            //fprintf(stdout, "El tiempo pulsado %f.\n", *ptr_tiempo_pulsado);
            while ((tiempo_inicio_led_prender < *ptr_tiempo_pulsado))
            {
                /*digitalWrite(LED, 0);*/
                gettimeofday(&tf, NULL);
                tiempo_inicio_led_prender = (tf.tv_sec - ti.tv_sec) * 1000 + (tf.tv_usec - ti.tv_usec) / 1000.0; // Obtenemos el tiempo que lleva el led prendido
                fprintf(stdout, "El tiempo pulsado %f.\n", *ptr_tiempo_pulsado);
                //fprintf(stdout, "La fecha es: %d.\n", *ptr_fecha_hora);
                //fprintf(stdout, "El nombre del pulsador %d.\n", *ptr_name);
            }
            /*digitalWrite(LED, 1);*/ // Prendemos el LED
            //printf("El periodo del LED es de : %g milisegundos\n", tiempo_inicio_led_prender);
            //fprintf(stdout, "La fecha es: %d.\n", *ptr_fecha_hora);
            //fprintf(stdout, "El nombre del pulsador %d.\n", *ptr_name);
            //fprintf(stdout, "El tiempo pulsado %f.\n", *ptr_tiempo_pulsado);
            *ptr_tiempo_pulsado=0;
            
        }
        
    }

    return 0;
}