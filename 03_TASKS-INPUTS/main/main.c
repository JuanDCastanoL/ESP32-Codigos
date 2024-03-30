#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"


#define GPIO_LED           (13)         // Número del pin del led.

// Tarea Blink
void blink_led(void *pvParameters)              //La función xTaskCreate espera un puntero 
{
    for(;;)
    {
        gpio_set_level(GPIO_LED, 0);
        vTaskDelay(550 / portTICK_PERIOD_MS);   
            
        gpio_set_level(GPIO_LED, 1);         
        vTaskDelay(300 / portTICK_PERIOD_MS);   
    }
}


void app_main(void)
{
    // Configurar el GPIO
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << GPIO_LED);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);


    // Crear tarea
    TaskHandle_t xHandle = NULL;
    xTaskCreate(blink_led, "B_LED", 1024, NULL, tskIDLE_PRIORITY, &xHandle);
    configASSERT( xHandle );

    for(;;)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

}