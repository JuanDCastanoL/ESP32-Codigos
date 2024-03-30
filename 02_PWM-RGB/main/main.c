#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"

#include "rgb.h"


// Definiendo los pines de los Leds
#define GPIO_LED_1_R           (13)
#define GPIO_LED_1_G           (12)
#define GPIO_LED_1_B           (14)
#define GPIO_LED_2_R           (27)
#define GPIO_LED_2_G           (26)
#define GPIO_LED_2_B           (25)



void app_main(void)
{

    // Inicializando el timer y los canales
    RGB_TIMER_INIT();
    led_rgb_t led_rgb_1 = RGB_CHANNEL_INIT_1(GPIO_LED_1_R, GPIO_LED_1_G, GPIO_LED_1_B);
    led_rgb_t led_rgb_2 = RGB_CHANNEL_INIT_2(GPIO_LED_2_R, GPIO_LED_2_G, GPIO_LED_2_B);
    

    while(1)
    {

        // Modificando la intensidad de los RGB
        RGB_CHANGE(led_rgb_1, 0, 14, 0);
        RGB_CHANGE(led_rgb_2, 0, 0, 20);        

        vTaskDelay(700 / portTICK_PERIOD_MS);  
    
        RGB_CHANGE(led_rgb_2, 70, 117, 50);

        vTaskDelay(600 / portTICK_PERIOD_MS);

        RGB_CHANGE(led_rgb_1, 206, 200, 91);
        RGB_CHANGE(led_rgb_2, 100, 36, 100); 

        vTaskDelay(500 / portTICK_PERIOD_MS);  

        RGB_CHANGE(led_rgb_1, 66, 30, 151);

        vTaskDelay(400 / portTICK_PERIOD_MS);   
        
    }

}
