#include <wiringPi.h> // Inicializo libreria para controlar los pines
#include <stdio.h>    // Inicializo libreria para imprimir por pantalla
#include <time.h>     // Inicializo libreria para calcular los tiempos

#define LED 17 // El pin donde se conecta el Led

int main(void)
{
  clock_t start; // Declaro variable para guardar los clocks
  int i = 0;     // Declaro variable hacer el while infinito

  wiringPiSetupGpio();  // Establezco conexion con los pines
  pinMode(LED, OUTPUT); // Declaro al pin 17 como pin de salida
  start = clock();      // Guardo la cantidad de clocks transcurridos
  while (1 > i)
  {
    if ((((double)(clock() - start) / (CLOCKS_PER_SEC))) <= 0.5) //Voy contando los clocks hasta que pase medio segundo
    {
      digitalWrite(LED, 1); // Prendo el Led
      printf("Prendo\n");   // Imprimo en pantalla
    }
    else
    {
      digitalWrite(LED, 0); // Apago el Led
      printf("Apago\n");    // Imprimo en pantalla
      if ((((double)(clock() - start) / (CLOCKS_PER_SEC))) >= 1) //Voy contando los clocks hasta que pase un segundo
      {
        start = clock(); // "Reseteo los segundos" para volver a prender el led
      }
    }
  }

  return 0;
}