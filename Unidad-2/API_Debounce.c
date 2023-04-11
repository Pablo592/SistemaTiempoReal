/*
 * API_debound.c
 *
 *  Created on: 9 nov. 2021
 *      Author: luis
 */
#include "API_Debounce.h"
#include "API_delay.h"

static delay_t delay;


#define DELAY_TIME 40

void buttonReleased(void);
void  buttonPressed(void);
// Función Inicializar MEF

bool_t debounceInit(void){

	  /*Initialize Push Button */
	  BSP_PB_Init(BUTTON_USER,BUTTON_MODE_GPIO);
	  BSP_LED_Init(LED1);
	  BSP_LED_Init(LED2);
	  BSP_LED_Init(LED3);
	  delayInit(&delay,DELAY_TIME );
	  debounceState = BUTTON_UP;
	  secuencia=false;
      return true;
}

// Función Actualizar MEF
void debounceUpdate(void){
    switch (debounceState) {
        case BUTTON_UP:
             if(BSP_PB_GetState(BUTTON_USER))
             { delayRead(&delay);
               debounceState=BUTTON_FALLING;
             }



        break;
        case BUTTON_FALLING:
        	 if(delayRead(&delay))
        	  {if(BSP_PB_GetState(BUTTON_USER))
        	      {  buttonPressed();
        	         debounceState=BUTTON_DOWN;
        		  }
        	   else{debounceState=BUTTON_UP;}
        	  }

        break;
        case BUTTON_DOWN:
        	 if(!BSP_PB_GetState(BUTTON_USER))
        	             { delayRead(&delay);
        	               debounceState=BUTTON_RISING;
        	             }

             break;
        case BUTTON_RISING:
        	 if(delayRead(&delay))
        	        	  {if(!BSP_PB_GetState(BUTTON_USER))
        	        	      {  buttonReleased();
        	        	         debounceState=BUTTON_UP;
        	        		  }
        	        	   else{debounceState=BUTTON_DOWN;}
        	        	  }

        	break;
        default:
                    	debounceInit();
        break;
        }
}

void buttonReleased(void)
{ //BSP_LED_Toggle(LED1);
  //BSP_LED_Toggle(LED2);
}
void  buttonPressed(void)
{
	 //BSP_LED_Toggle(LED1);
     //BSP_LED_Toggle(LED3);
	if(secuencia)
		secuencia=false;
	else{secuencia=true;}
}
