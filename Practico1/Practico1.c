// #include <wiringPi.h>
#include <stdio.h>
#include <time.h>

#define LED 17

int main(void)
{
  clock_t start, stop;
  int i = 0;

  time_t rawtime;
  struct tm *timeinfo;

  // wiringPiSetupGpio();
  // pinMode(LED, OUTPUT);
  start = clock();
  while (1 > i)
  {
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    if ((((double)(clock() - start) / (CLOCKS_PER_SEC))) <= 0.5)
    {
      //  digitalWrite(LED, 1);
      printf("Prendo%d\n",timeinfo->tm_sec);
    }
    else
    {
      //  digitalWrite(LED, 0);
      printf("Apago%d\n",timeinfo->tm_sec);
      if ((((double)(clock() - start) / (CLOCKS_PER_SEC))) >= 1)
      {
        start = clock();
      }
    }
  }

  return 0;
}