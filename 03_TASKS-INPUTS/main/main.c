#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "driver/gpio.h"                // Manejo GPIO
#include "esp_log.h"                    // Mensajes de log
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"              // Tareas
#include "freertos/queue.h"             // Colas



#define GPIO_LED           (13)                  // Número del pin del led.
#define GPIO_BOTON         (33)                  // Número del pin del botón.

#define FLAG_INTR_BOTON     (0)                  // Bandera de la interrupcion del botón

TaskHandle_t  xHandle_task_led    = NULL;        // Tarea vacia 
QueueHandle_t queue_gpio_boton    = NULL;        // Cola vacia

static const char *TAG = "EXAMPLE";



// Inicializacion pines
void init_pins(void)
{    
    // Configurar el GPIO del botón
    gpio_config_t io_conf_btt = {};
    io_conf_btt.pin_bit_mask = (1ULL << GPIO_BOTON);
    io_conf_btt.mode = GPIO_MODE_INPUT;
    io_conf_btt.intr_type = GPIO_INTR_NEGEDGE;
    io_conf_btt.pull_down_en = 0;
    io_conf_btt.pull_up_en = 1;
    gpio_config(&io_conf_btt);

    // Configurar el GPIO del led
    gpio_config_t io_conf_led = {};
    io_conf_led.pin_bit_mask = (1ULL << GPIO_LED);
    io_conf_led.mode = GPIO_MODE_OUTPUT;
    io_conf_led.intr_type = GPIO_INTR_DISABLE;
    io_conf_led.pull_down_en = 0;
    io_conf_led.pull_up_en = 0;
    gpio_config(&io_conf_led);
}


// Configuración de la interrupción del botón
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t) arg;                     // Lo que se enviará a la cola
    xQueueSendFromISR(queue_gpio_boton, &gpio_num, NULL);
}


// Tarea para el manejo del led
void task_led(void *arg) 
{
    uint32_t gpio_num_Queue = 0;         // Variable donde se almacena lo que llegue de la cola 
    bool estado_prog = false;            // Estado del programa   0:OFF   1:BLINK
    bool estado_led = false;             // Estado del led        0:OFF   1:ON

    for(;;){

        // Verifica si hay algo en la cola
        if(xQueuePeek(queue_gpio_boton, &gpio_num_Queue, 0) == pdPASS){
            // Si hay algo en la cola, recibe el elemento
            if(xQueueReceive(queue_gpio_boton, &gpio_num_Queue, 0) == pdPASS){
                // Si no se usa xQueuePeek poner de tiempo 10 / portTICK_PERIOD_MS
                
                // Cambiar el estado del programa al opuesto
                estado_prog = !estado_prog;

                ESP_LOGI(TAG, "Estado: %d", estado_prog);
                vTaskDelay(300 / portTICK_PERIOD_MS);        // Espera por rebotes
                xQueueReset(queue_gpio_boton);               // Limpia completamente la cola
            }
        }

        // Dependiendo del estado modifica el funcionamiento del led
        if (estado_prog){

            estado_led = !estado_led;
            gpio_set_level(GPIO_LED, estado_led);

            vTaskDelay(1000 / portTICK_PERIOD_MS);

        }else{

            estado_led = false;
            gpio_set_level(GPIO_LED, estado_led);
        }
        
    }
}




void app_main(void)
{
    init_pins();

    // Configurar Interrupción
    gpio_install_isr_service(FLAG_INTR_BOTON);
    gpio_isr_handler_add(GPIO_BOTON, gpio_isr_handler, NULL);


    // Crear tarea
    xTaskCreate(task_led, "B_LED", 2048, NULL, 0, &xHandle_task_led);
    configASSERT( xHandle_task_led );         //verifica si se creo bien , sino interrumpe la ejecución del programa

    //Tarea manejada por el nucleo 1
    //xTaskCreatePinnedToCore(task_led, "B_LED", 2048, NULL, 0, &xHandle_task_led, 1); 


    // Crear Cola
    queue_gpio_boton = xQueueCreate(5, sizeof(uint32_t));
    configASSERT( queue_gpio_boton );

    for(;;){
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "Seg");
    }


}