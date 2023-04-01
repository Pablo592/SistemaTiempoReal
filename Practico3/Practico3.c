#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>

#define PWM_PIN 1 // Definimos el pin de salida que se utilizará para la señal PWM


int main(void)
{

    float grados;
    grados = 0;
   
    wiringPiSetup();     // Inicializamos la biblioteca WiringPi
    pinMode(PWM_PIN, PWM_OUTPUT); // Se establece que el pin sera de salida
    digitalWrite(PWM_PIN, 0);  // Se utiliza para escribir un valor digital (ALTO o BAJO) en el pin de la raspberry
    softPwmCreate(PWM_PIN, 0, 200); // crea una señal de modulación de ancho de pulso (PWM) impulsada por software en un pin GPIO específico. 
                                    //softPwmCreate(int pin, int initialValue, int pwmRange);
   
    float microsegundos;

    while (1)
    {
        printf("Incerte la cantidad de grados que debe girar el servo \n");
        scanf("%f", &grados);
        microsegundos = (((1 / 180) * grados) + 1) * 10;
        printf("microsegundos %f\n", microsegundos);
        softPwmWrite(PWM_PIN, (microsegundos)); // Se establece cuanto debe durar la fase de la señal digital
        delay(1000);
    }

    return 0;
}