#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"


#define GPIO_LED           (13)         // Número del pin del led.

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

    while(1)
    {
        
        gpio_set_level(GPIO_LED, 0);            // Apaga el Led
        vTaskDelay(500 / portTICK_PERIOD_MS);   // Esperar 500 ms
        
        gpio_set_level(GPIO_LED, 1);            // Enciende el led
        vTaskDelay(400 / portTICK_PERIOD_MS);   // Esperar 400 ms
    }

}