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
//TaskHandle_t   xHandle_task_ntc    = NULL;
adc_oneshot_unit_handle_t adc1_handle;
led_rgb_t led_rgb_1;
led_rgb_t led_rgb_2;

#define ATTEN_ADC ADC_ATTEN_DB_12
#define BITWH_ADC ADC_BITWIDTH_12
#define CANAL_ADC_POT ADC_CHANNEL_4            // GPIO32 - ADC1 CHANNEL4 - Potenciometro
//#define CANAL_ADC_NTC ADC_CHANNEL_4
#define SAMPLES               (20) 

static const char *TAG = "EXAMPLE";

static int adc_raw[2];
//static int voltage[2];                        // Por si se desea cambiar lectura del ADC a voltaje

// Definiendo los pines de los Leds
#define GPIO_LED_1_R           (13)
#define GPIO_LED_1_G           (12)
#define GPIO_LED_1_B           (14)
#define GPIO_LED_2_R           (27)
#define GPIO_LED_2_G           (26)
#define GPIO_LED_2_B           (25)


void task_pot(void *arg)
{
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
       
            xSemaphoreGive(xSemaphore);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

        }
    } 
        
}

/*
void task_ntc(void *arg)
{

}
*/




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

    xTaskCreatePinnedToCore(task_pot, "T_POT", 8192, NULL, 0, &xHandle_task_pot, 1);
    configASSERT( xHandle_task_pot );

    //xTaskCreatePinnedToCore(task_ntc, "T_NTC", 2048, NULL, 0, &xHandle_task_ntc, 1);
    //configASSERT( xHandle_task_ntc );

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
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, CANAL_ADC_POT, &config));   // Conf canal 4 - Potenciometro
    //ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, CANAL_ADC_POT, &config));   // Conf canal 4 - 

    /*    Si se requiere de la curva de calibración
    adc_cali_handle_t adc1_cali_chan4_handle = NULL;
    bool do_calibration1_chan4 = example_adc_calibration_init(ADC_UNIT_1, CANAL_ADC_POT, ATTEN_ADC, &adc1_cali_chan4_handle);   // Calib ADC1-Canal4
    */

    //---------------- ↑ ADC Set-Up ↑ -------------------//


    //---------------- ↓ RGB Set-Up ↓------------------//

    RGB_TIMER_INIT();
    led_rgb_1 = RGB_CHANNEL_INIT_1(GPIO_LED_1_R, GPIO_LED_1_G, GPIO_LED_1_B);
    //led_rgb_2 = RGB_CHANNEL_INIT_2(GPIO_LED_2_R, GPIO_LED_2_G, GPIO_LED_2_B);

    //---------------- ↑ RGB Set-Up ↑ -------------------//

    while(1){
           
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}






/*    Si se desea pasar la lectura a voltaje teniendo en cuenta la curva de calibración
if (do_calibration1_chan4) {
    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_cali_chan4_handle, adc_raw[0], &voltage[0]));
    ESP_LOGI(TAG, "ADC%d Channel[%d] Cali Voltage: %d mV", ADC_UNIT_1 + 1, CANAL_ADC_POT, voltage[0]);
}
*/
