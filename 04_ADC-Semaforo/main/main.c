#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h" 

#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#include "ESP32Utilidades.h"
#include "rgb.h"


SemaphoreHandle_t xSemaphore       = NULL;
TaskHandle_t   xHandle_task_pot    = NULL;
TaskHandle_t   xHandle_task_ldr    = NULL;
adc_oneshot_unit_handle_t adc1_handle;
led_rgb_t led_rgb_1;


#define ATTEN_ADC ADC_ATTEN_DB_12
#define BITWH_ADC ADC_BITWIDTH_12
//#define CANAL_ADC_POT ADC_CHANNEL_4            // GPIO32 - ADC1 CHANNEL4 - Potenciometro
#define CANAL_ADC_LDR ADC_CHANNEL_6            // GPIO34 - ADC1 CHANNEL6 - Resistencia LDR 
#define SAMPLES               (20) 

static const char *TAG = "EXAMPLE";

static int adc_raw[2];
//static int voltage[2];                        // Por si se desea cambiar lectura del ADC a voltaje

// Definiendo los pines de los Leds
#define GPIO_LED_1_R           (13)
#define GPIO_LED_1_G           (12)
#define GPIO_LED_1_B           (14)


/*
void task_pot(void *arg)
{
    int cont = 0;
    for(;;){

        if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE){

            adc_oneshot_read_multisample(adc1_handle, CANAL_ADC_POT, &adc_raw[0], SAMPLES);
            //ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, CANAL_ADC_POT, adc_raw[0]);

            
            if (adc_raw[0] < 1365) {
                RGB_CHANGE(led_rgb_1, 0, 0, 255); 
            } else if (adc_raw[0] < 2730) {
                RGB_CHANGE(led_rgb_1, 0, 255, 0); 
            } else {
                RGB_CHANGE(led_rgb_1, 255, 0, 0);    
            }
       

            if (cont == 10) {
                cont = 0;
                xSemaphoreGive(xSemaphore);
            } else {
                cont++;
                vTaskDelay(500 / portTICK_PERIOD_MS); 
            }
                            

        }
    } 
        
}
*/


void task_ldr(void *arg)
{
    
    for (;;)
    {
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE){

                        
            adc_oneshot_read_multisample(adc1_handle, CANAL_ADC_LDR, &adc_raw[1], SAMPLES);
            ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, CANAL_ADC_LDR, adc_raw[1]);

            if (adc_raw[1] < 900){
                RGB_CHANGE(led_rgb_1, 170, 0, 25);
            } else if (adc_raw[1] < 1800){
                RGB_CHANGE(led_rgb_1, 70, 30, 25);
            } else if (adc_raw[1] < 2600){
                RGB_CHANGE(led_rgb_1, 30, 70, 25);
            } else {
                RGB_CHANGE(led_rgb_1, 0, 170, 25);
            }


            xSemaphoreGive(xSemaphore);
            
            vTaskDelay(500 / portTICK_PERIOD_MS); 
            
            
        }
    }
    
}





void app_main(void)
{
    //---------------- ↓ Semaforo Set-Up ↓------------------//

    xSemaphore = xSemaphoreCreateBinary();
    /*
    if(xSemaphore != NULL){
        ESP_LOGE(TAG, "El semáforo no se ha creado correctamente.");
    }*/
    xSemaphoreGive(xSemaphore);

    //---------------- ↑ Semaforo Set-Up ↑ -------------------//


    //---------------- ↓ Tareas Set-Up ↓------------------//

    //xTaskCreatePinnedToCore(task_pot, "T_POT", 8192, NULL, 0, &xHandle_task_pot, 1);
    //configASSERT( xHandle_task_pot );

    xTaskCreatePinnedToCore(task_ldr, "T_LDR", 2048, NULL, 0, &xHandle_task_ldr, 1);
    configASSERT( xHandle_task_ldr );

    //---------------- ↑ Tareas Set-Up ↑ -------------------//


    //---------------- ↓ ADC Set-Up ↓------------------//

    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));     // Activa ADC1

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = BITWH_ADC,
        .atten = ATTEN_ADC,
    };
    //ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, CANAL_ADC_POT, &config));   // Conf canal 4 - Potenciometro
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, CANAL_ADC_LDR, &config));   // Conf canal 6 - Resistencia LDR

    //---------------- ↑ ADC Set-Up ↑ -------------------//


    //---------------- ↓ RGB Set-Up ↓------------------//

    RGB_TIMER_INIT();
    led_rgb_1 = RGB_CHANNEL_INIT_1(GPIO_LED_1_R, GPIO_LED_1_G, GPIO_LED_1_B);

    //---------------- ↑ RGB Set-Up ↑ -------------------//

    while(1){
           
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}



/*    Si se requiere de la curva de calibración, en el set up
adc_cali_handle_t adc1_cali_chan4_handle = NULL;
bool do_calibration1_chan4 = example_adc_calibration_init(ADC_UNIT_1, CANAL_ADC_POT, ATTEN_ADC, &adc1_cali_chan4_handle);   // Calib ADC1-Canal4
*/


/*    Si se desea pasar la lectura a voltaje teniendo en cuenta la curva de calibración, donde se requiera
if (do_calibration1_chan4) {
    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_cali_chan4_handle, adc_raw[0], &voltage[0]));
    ESP_LOGI(TAG, "ADC%d Channel[%d] Cali Voltage: %d mV", ADC_UNIT_1 + 1, CANAL_ADC_POT, voltage[0]);
}
*/
