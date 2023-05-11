#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "driver/mcpwm.h"
#include "esp_log.h"

#define SERVO_MIN_PULSEWIDTH 500  //Duración mínima del pulso en microsegundos
#define SERVO_MAX_PULSEWIDTH 2500 //Duración máxima del pulso en microsegundos
#define SERVO_MAX_DEGREE 180      //Ángulo máximo del servo

static const char *TAG = "servo";

void servo_task(void)
{
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, GPIO_NUM_26); //Configuración del pin GPIO para el servo
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 50;    //Frecuencia PWM
    pwm_config.cmpr_a = 0;        //Ciclo de trabajo inicial del servo en 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config); //Inicialización del controlador PWM

    adc1_config_width(ADC_WIDTH_BIT_12); //Configuración del ancho de bits del ADC
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11); //Configuración de la entrada analógica del ADC

    while(1)
    {
        int adc_value = adc1_get_raw(ADC1_CHANNEL_0); //Lectura del valor analógico del ADC
        int duty_us = (adc_value / 4095.0) * (SERVO_MAX_PULSEWIDTH - SERVO_MIN_PULSEWIDTH) + SERVO_MIN_PULSEWIDTH; //Cálculo del ciclo de trabajo para el servo

        mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty_us); //Mover el servo al ángulo correspondiente

        vTaskDelay(pdMS_TO_TICKS(20)); //Retardo para evitar la sobrecarga del procesador
    }
}

void app_main()
{
    xTaskCreate(servo_task, "servo_task", 2048, NULL, 5, NULL); //Creación de la tarea para el servo
}
