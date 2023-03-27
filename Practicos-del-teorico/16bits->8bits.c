#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
// Definimos el pin de salida que se utilizará para la señal PWM
#define PWM_PIN 1

int main(void) {

    float grados;
    grados = 0;
    // Inicializamos la biblioteca WiringPi y configuramos el pin de salida como PWM
   // wiringPiSetupGpio();
   // pinMode(PWM_PIN, PWM_OUTPUT);
    wiringPiSetup();
    pinMode(PWM_PIN,PWM_OUTPUT);
    digitalWrite(PWM_PIN,0);
    softPwmCreate(PWM_PIN,0,200);   
    // Configuramos la frecuencia de la señal PWM
   // pwmSetMode(PWM_MODE_MS);
   // pwmSetClock(384);
   // pwmSetRange(1000);
    float microsegundos;

    while(1) {
        printf("Incerte la cantidad de grados que debe girar el servo \n");
        scanf("%f", &grados);
        // Enviamos el pulso mínimo al servo
        microsegundos = (((1/180)*grados)+1)*10
printf("microsegundos %f\n",microsegundos);
	softPwmWrite(PWM_PIN,(microsegundos));
       // pwmWrite(PWM_PIN, microsegundos);

        // Esperamos un tiempo (por ejemplo, 1 segundo)
        delay(1000);
    }

    return 0;
}

/*
Este programa utiliza la biblioteca WiringPi para generar una señal PWM de 50 Hz en el pin 18 de la Raspberry Pi 3. 
Luego, envía pulsos de ancho variable (entre 500 y 2500 microsegundos) al servo SG90 utilizando la función pwmWrite().
*/
