#include <wiringPi.h>
#include <stdio.h>

#define LED 17   
#define TIEMPO 500 //tiempo en milisegundos

int main (void)
{
  int i=0;
  wiringPiSetupGpio();
  pinMode (LED, OUTPUT) ;
  while (1>i)
  {
    digitalWrite (LED, 1) ; delay (TIEMPO) ;
    printf("Prendo/n");
    digitalWrite (LED, 0) ; delay (TIEMPO) ;
    printf("Apago/n");
  }
  return 0 ;
}