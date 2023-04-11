
#include "API_delay.h"


void delayInit( delay_t * delay, tick_t duration ){
delay->duration= duration;
delay->running=0;
}


bool_t delayRead( delay_t *delay ){

	bool_t tiempo_cumplido=false;
    uint32_t dif_valor=0;
  if(!delay->running)
     {delay->startTime = HAL_GetTick();
      delay->running=true;
     }
  else{
	   dif_valor=HAL_GetTick()-delay->startTime;
	   if(dif_valor>=delay->duration)
	      {tiempo_cumplido=true;
	       delay->running=false;
		  }
	   else{tiempo_cumplido=false;}
       }
 return tiempo_cumplido;
}

void delayWrite( delay_t * delay, tick_t duration ){
	delay->duration=duration;
}

