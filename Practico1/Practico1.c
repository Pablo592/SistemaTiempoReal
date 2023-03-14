//#include <wiringPi.h>
#include <stdio.h>
#include <time.h>

#define LED 17

int main(void)
{

  time_t rawtime;
  struct tm *timeinfo;

  int i = 0;
  //wiringPiSetupGpio();
  //pinMode(LED, OUTPUT);
  while (1 > i)
  {
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    if (timeinfo->tm_sec % 2 == 0)
    {
    //  digitalWrite(LED, 1);
      printf("Prendo\n");
    }
    else
    {
    //  digitalWrite(LED, 0);
      printf("Apago\n");
    }
  }

  return 0;
}