#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h" 


#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"


#include "ESP32Utilidades.h"



static const char *TAG = "EXAMPLE";

#define ATTEN_ADC ADC_ATTEN_DB_12
#define BITWH_ADC ADC_BITWIDTH_12
#define CANAL_ADC ADC_CHANNEL_4        // GPIO32 - ADC1 CHANNEL4
#define SAMPLES (20) 

static int adc_raw[2];
static int voltage[2];


void app_main(void)
{
    //---------------- ↓ ADC Set-Up ↓------------------//

    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));     // Activa ADC1

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = BITWH_ADC,
        .atten = ATTEN_ADC,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, CANAL_ADC, &config));   // Conf canal 4
    
    adc_cali_handle_t adc1_cali_chan4_handle = NULL;
    bool do_calibration1_chan4 = example_adc_calibration_init(ADC_UNIT_1, CANAL_ADC, ATTEN_ADC, &adc1_cali_chan4_handle);   // Calib ADC1-Canal4

    //---------------- ↑ ADC Set-Up ↑ -------------------//

    while(1){
                
        adc_oneshot_read_multisample(adc1_handle, CANAL_ADC, &adc_raw[0], SAMPLES);
        
        ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, CANAL_ADC, adc_raw[0]);
        if (do_calibration1_chan4) {
            ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_cali_chan4_handle, adc_raw[0], &voltage[0]));
            ESP_LOGI(TAG, "ADC%d Channel[%d] Cali Voltage: %d mV", ADC_UNIT_1 + 1, CANAL_ADC, voltage[0]);
        }
        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}


