#include "ESP32Utilidades.h"

bool example_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(TAG0, "calibration scheme version is %s", "Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .chan = channel,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(TAG0, "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

    *out_handle = handle;
    if (ret == ESP_OK) {
        ESP_LOGI(TAG0, "Calibration Success");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        ESP_LOGW(TAG0, "eFuse not burnt, skip software calibration");
    } else {
        ESP_LOGE(TAG0, "Invalid arg or no memory");
    }

    return calibrated;
}

void example_adc_calibration_deinit(adc_cali_handle_t handle)
{
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    ESP_LOGI(TAG0, "deregister %s calibration scheme", "Curve Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(handle));

#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    ESP_LOGI(TAG0, "deregister %s calibration scheme", "Line Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));
#endif
}


/**
 * Realiza múltiples lecturas de un canal ADC especificado y los promedia.
 * 
 * @param handle adc_oneshot_unit_handle_t: El identificador de la unidad ADC.
 * @param chan adc_channel_t: El canal ADC específico que se va a leer.
 * @param out_raw int: Puntero donde se almacenará el valor promedio de las lecturas.
 * @param samples int: El número de muestras a tomar y promediar.
 * 
 * @return NULL
 * 
*/
void adc_oneshot_read_multisample(adc_oneshot_unit_handle_t handle, adc_channel_t chan, int *out_raw, int samples){
    
    if(samples<1){
        ESP_LOGW(TAG0, "Samples must be greater than 0, exiting of multisample.");
        return;

    }else{

        int raw = 0;
        int averaged = 0;

        for(int i=0; i < samples; i++){
            ESP_ERROR_CHECK(adc_oneshot_read(handle, chan, &raw));
            averaged += raw;
        }
        averaged /= samples;

        *out_raw = averaged;    
    }
}