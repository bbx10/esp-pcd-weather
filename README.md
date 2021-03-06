# esp-pcd-weather
ESP8266 Arduino connected to PCD8544/5110 LCD display showing weather from Weather Underground

![Weather on LCD screen](./esp-pcd-weather.jpg)

## Hardware Parts ##

* [Adafruit Huzzah ESP8266](https://www.adafruit.com/products/2471)
* [Adafruit PCD8544/5110 display](https://www.adafruit.com/product/338)
* [Adafruit USB to TTL serial cable](https://www.adafruit.com/products/954)

## Connections ##

USB TTL    |Huzzah ESP8266|PCD8544/Nokia 5110 |Description
-----------|-----------|-----------|-------------------------------------------------------------
           |GND        |GND        |Ground
           |3V         |VCC        |3.3V from Huzzah to display
           |14         |CLK        |Output from ESP SPI clock
           |13         |DIN        |Output from ESP SPI MOSI to display data input
           |12         |D/C        |Output from display data/command to ESP
           |#5         |CS         |Output from ESP to chip select/enable display
           |#4         |RST        |Output from ESP to reset display
           |15         |LED        |3.3V to turn backlight on, GND off
GND (blk)  |GND        |           |Ground
5V  (red)  |V+         |           |5V power from PC or charger
TX  (green)|RX         |           |Serial data from IDE to ESP
RX  (white)|TX         |           |Serial data to ESP from IDE

## Dependencies ##

* [Fork of Adafruit PCD8544 library with changes for ESP8266]
  (https://github.com/bbx10/Adafruit-PCD8544-Nokia-5110-LCD-library/tree/esp8266).
  Use the esp8266 branch!
* Adafruit GFX library. Use the Arduino IDE Library Manager to get the latest version
* Arduino JSON library. Use the Arduino IDE Library Manager to get the latest version
* ESP8266 board package. Use the stable version. https://github.com/esp8266/arduino#stable-version-

## Recently tested versions ##

The libraries and board package should be installed using the IDE library and
board managers.

* IDE 1.6.8
* Adafruit GFX library 1.1.5
* ArduinoJSON library 5.1.1
* ESP8266 board package 2.1.0
* Fork of Adafruit PCD8544 library 1.0.0 with ESP8266 changes. Download the ZIP file from the
following link then use the IDE `Sketch | Include Library | Add .ZIP Library` option to install it.

https://github.com/bbx10/Adafruit-PCD8544-Nokia-5110-LCD-library/tree/esp8266
