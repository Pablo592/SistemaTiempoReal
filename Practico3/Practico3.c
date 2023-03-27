#include <stdio.h>
#include <wiringPi.h>

// Definimos el pin de salida que se utilizará para la señal PWM
#define PWM_PIN 18

int main(void) {

    float grados;
    grados = 0;
    // Inicializamos la biblioteca WiringPi y configuramos el pin de salida como PWM
    wiringPiSetupGpio();
    pinMode(PWM_PIN, PWM_OUTPUT);

    // Configuramos la frecuencia de la señal PWM
    pwmSetMode(PWM_MODE_MS);
    pwmSetClock(50);
    pwmSetRange(20);
    float milisegundos;

    while(1) {
        printf("Incerte la cantidad de grados que debe girar el servo \n")
        scanf("%lf", &grados);
        // Enviamos el pulso mínimo al servo
        milisegundos = (grados*0.5)/90;


        pwmWrite(PWM_PIN, milisegundos);

        // Esperamos un tiempo (por ejemplo, 1 segundo)
        delay(1000);
    }

    return 0;
}

/*
Este programa utiliza la biblioteca WiringPi para generar una señal PWM de 50 Hz en el pin 18 de la Raspberry Pi 3. 
Luego, envía pulsos de ancho variable (entre 500 y 2500 microsegundos) al servo SG90 utilizando la función pwmWrite().
*/