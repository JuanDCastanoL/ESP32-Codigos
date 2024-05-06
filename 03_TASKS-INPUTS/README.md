# 03_TASKS-INPUT

Este programa utiliza un botón para controlar el comportamiento de un Led, alternando entre un estado apagado y un estado de parpadeo.

## Montaje Físico

Se requiere un botón conectado a los pines GPIO 33 y tierra, así como un LED conectado al pin GPIO 13,junto con una resistencia de protección que se conecta a tierra.

## Lógica del Programa

El pin del botón se configura con una resistencia Pull-Up y se establece una interrupción por flanco de bajada. Cuando se detecta la interrupción, se envía el número del pin que la generó a la cola `queue_gpio_boton`. Mientras tanto, la tarea `task_led` monitorea constantemente esta cola. Cuando recibe un valor, cambia el estado del programa, siendo 0 para apagado y 1 para parpadeo. Luego, se utiliza un condicional para determinar el comportamiento del LED.

> Se recomienda consultar la documentación oficial de FreeRTOS para obtener información detallada sobre el manejo de colas y tareas [aqui](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/freertos_idf.html).

Es importante tener en cuenta que los pines 34-39 no tienen la electrónica necesaria para configurar las entradas como Pull-Up/Down.