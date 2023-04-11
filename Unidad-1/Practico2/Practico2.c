#include <wiringPi.h>       //
#include <stdio.h>              //
#include <stdlib.h>                 //
#include <unistd.h>                 // Inicializo librerias
#include <linux/i2c-dev.h>          //
#include <sys/ioctl.h>          //
#include <fcntl.h>         //

#define LED1 17 // El pin donde se conecta el Led
#define LED2 18 // El pin donde se conecta el Led

#define AHT10_ADDRESS 0x38 // AHT10 I2C address

void ledTemp(int medicion); // Funcion que enciende el led en base a la temperatura 
void ledHum(int medicion);  // Funcion que enciende el led en base a la humedad 

int main()
{
    wiringPiSetupGpio();   // Establezco conexion con los pines
    pinMode(LED1, OUTPUT); // Declaro al pin 17 como pin de salida
    pinMode(LED2, OUTPUT); // Declaro al pin 18 como pin de salida

    int file;
    char *filename = "/dev/i2c-1"; // I2C bus device file
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

    // Send command to measure temperature and humidity
    char command[3] = {0xAC, 0x33, 0x00}; //Busco en memoria la humedad y temperatura detectada por el sensor

    float cur_temp, ctmp;
    while (1)   // El sensor recopila datos sin parar
    {
        write(file, command, 3);
        usleep(50000); // Se espera un tiempo a que las mediciones puedan completarse (50ms)
        char data[6];
        read(file, data, 6);

        cur_temp = (((data[3] & 0x0F) << 16) | (data[4] << 8) | data[5]);
        cur_temp = ((cur_temp * 200) / 1048576) - 50;
        printf("Temperature: %2.2f\n", cur_temp);   // Se imprime la temperatura monitoreada por consola
        ctmp = ((data[1] << 16) | (data[2] << 8) | data[3]) >> 4;
        ctmp = ctmp * 100 / 1048576;
        printf("Humidity: %1.f %\n", ctmp); // Se imprime la humedad monitoreada por consola

        ledTemp(cur_temp);  //Esta funcion enciende un led cuando la temperatura supera los 35 grados
        ledHum(ctmp);       //Esta funcion enciende un led cuando la humedad supera el 70%
    }

    close(file);
    return 0;
}

void ledTemp(medicion)
{

    if (medicion > 35)
    {
        // prendo led
        digitalWrite(LED1, 1); // Prendo el Led
        printf("Prendo Led")
    }
    else
    {
        digitalWrite(LED1, 0);
        printf("Apago Led")
    }
}

void ledHum(int medicion)
{
    if (medicion > 70)
    {
        // prendo led
        digitalWrite(LED2, 1); // Prendo el Led
        printf("Prendo Led")
    }
    else
    {
        digitalWrite(LED2, 0);
        printf("Apago Led")
    }
}