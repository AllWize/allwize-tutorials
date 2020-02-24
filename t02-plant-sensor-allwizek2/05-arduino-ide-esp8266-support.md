# Install support for the AllWize G1 in the Arduino IDE

The AllWize G1 is Wemos D1 ESP8266-based board and an AllWize K1 shield. To be able to code it from the Arduino IDE you will have to add support for ESP8266 in it. This is very simple.

Starting with 1.6.4, Arduino allows installation of third-party platform packages using Boards Manager. We have packages available for Windows, Mac OS, and Linux (32 and 64 bit).

* Start Arduino and open Preferences window.
* Enter https://arduino.esp8266.com/stable/package_esp8266com_index.json into Additional Board Manager URLs field. You can add multiple URLs, separating them with commas.
* Open Boards Manager from `Tools > Board ... > Boards Manager` menu and find esp8266 platform
* Select the 2.5.0 version from a drop-down box.
* Click install button.
* Don’t forget to select the `LOLIN (WEMOS) D1 R2 & mini` board from `Tools > Board` menu after installation.

[Previous: Device code](04-device-code.md) |
[Tutorial menu](readme.md) |
[Next: Install gateway dependencies to the Arduino IDE](06-arduino-ide-gateway-dependencies.md)