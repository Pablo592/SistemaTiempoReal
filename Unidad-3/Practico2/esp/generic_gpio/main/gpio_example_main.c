#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc_cal.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "driver/mcpwm.h"
#include <driver/adc.h>

#define SERVO_MIN_PULSEWIDTH 500  //Duración mínima del pulso en microsegundos
#define SERVO_MAX_PULSEWIDTH 2500 //Duración máxima del pulso en microsegundos
#define SERVO_MAX_DEGREE 180      //Ángulo máximo del servo
#define ADC_CHANNEL ADC1_CHANNEL_6


void servo_task(void *pvParameter)
{
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, GPIO_NUM_26); //Configuración del pin GPIO para el servo
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 50;    //Frecuencia PWM
    pwm_config.cmpr_a = 0;        //Ciclo de trabajo inicial del servo en 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config); //Inicialización del controlador PWM

    adc1_config_width(ADC_WIDTH_BIT_12); //Configuración del ancho de bits del ADC ---> Resolucion del ancho de pulso
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_6); //Configuración de la entrada analógica del ADC

    while(1)
    {
        int adc_value = adc1_get_raw(ADC_CHANNEL); //Lectura del valor analógico del ADC
        printf("el valor analogico es : %d", adc_value);

        int duty_us = (adc_value / 4095.0) * (SERVO_MAX_PULSEWIDTH - SERVO_MIN_PULSEWIDTH) + SERVO_MIN_PULSEWIDTH; //Cálculo del ciclo de trabajo para el servo
        printf("angulo : %d", duty_us);

        mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty_us); //Mover el servo al ángulo correspondiente

        vTaskDelay(1000); //Retardo para evitar la sobrecarga del procesador
    }
}

void app_main()
{
    xTaskCreate(servo_task, "servo_task", 2048, NULL, 5, NULL); //Creación de la tarea para el servo
}
