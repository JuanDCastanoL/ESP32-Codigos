# 05_UART_RGB

Este código es para un programa en un ESP32 que utiliza FreeRTOS para manejar la comunicación UART y controlar un LED RGB.

Se un RGB cátodo comun conectado a los GPIO 12, 13 y 14, junto a sus resistencias de protección.

Se emplean librerias personalizadas `rgb.h` y `ESP32Utilidades.h`. La primera se usa para el manejo del timer y la segunda guarda la inicialización y el envio basico del UART.

> Se explica más detalladamente en el [main.c](https://github.com/JuanDCastanoL/ESP32-Codigos/blob/main/05_UART_RGB/main/main.c).

## UART

El ESP32-WROOM-32 tiene 3 UART's diferentes:

* UART0: GPIO1(Tx) y GPIO3(Rx). Conectado al USB diracto al PC.
* UART1: GPIO5(Tx) y GPIO4(Rx). Conectado a SPI (SP3-SP2), no recomendado.
* UART2: GPIO17(Tx) y GPIO16(Rx).

