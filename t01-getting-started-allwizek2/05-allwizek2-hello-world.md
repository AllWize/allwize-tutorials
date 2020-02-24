# AllWize K2 Hello World

Now you should be able to start programming your AllWize K2. A sketch to start with is the `moduleInfo` example you can find under `File > Examples > AllWize > moduleInfo`. This code just dumps the information from the radio module to the serial monitor, so you can check that the board is working just as expected.

    Pro tip:
    The `moduleInfo` sketch is crowded with conditional compilation chunks of code to work
    on different platforms since the K2 uses a SAMD microcontroller and the AllWize K1 shield
    can be used with an Atmel AVR like the Arduino Leonardo or an ESP8266, for instance.
    You can inspect the code to learn how to use the library with many different platforms.

Now connect your AllWize K2 board to your computer and select the right port under the `Tools > Port` menu. The AllWize K2 will present itself as an "Arduino MKRZERO" board. Once you select the port you can compile and upload the code to the board.

![Arduino IDE Build sketch](images/arduino-ide-build.png)

Once it says `Done uploading` you can open the Serial Monitor in the IDE (`Tools > Serial Monitor` or clicking the magnifier icon on the right of the toolbar) and it should output the radio module configuration:

```
AllWize - Module Info

Initializing radio module
Radio module OK

Module info:

Property                 Value
------------------------------
Module type              WIZE
UART speed               19200
Channel                  4
Power                    5
MBUS Mode                0x11
Sleep Mode               0
Data Rate                2400
Preamble Length          0
Control Field            0x44
Network Role             0
Install Mode             2
Manufacturer ID          0x4824
Unique ID                0x26011CA1
Device Type              0
Device Version           1
Part Number              RC1701HP-WIZE
Firmware Version         1.00
Req. Hardware Version    1.10
Serial Number            0x0000000000000000
Temperature (C)          22
Voltage (mV)             3420


Memory dump:

       00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 
------------------------------------------------------
0x00:  04 04 01 01 05 11 00 00 00 00 05 3C 00 00 80 80            <    
0x10:  7C 00 00 01 00 00 17 00 00 48 24 26 01 1C A1 01 |        H$&    
0x20:  00 01 01 00 00 00 00 04 FF 00 50 05 04 03 4B 00           P   K 
0x30:  05 08 00 01 05 00 00 00 00 00 00 44 00 02 14 00            D    
0x40:  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00                 
0x50:  00 00 93 93 93 93 DF DF DF DF 1F 1F 1F 1F 10 10                 
0x60:  10 10 0E 0E 0E 0E 10 10 10 10 01 01 01 01 15 15                 
0x70:  15 15 E6 E6 E6 E6 44 44 44 44 52 43 31 37 30 31       DDDDRC1701
0x80:  48 50 2D 57 49 5A 45 2C 31 2E 31 30 2C 31 2E 30 HP-WIZE,1.10,1.0
0x90:  30 20 20 20 20 20 20 20 20 20 00 00 00 00 00 00 0               
0xA0:  00 00 FF FF FF FF FF FF FF FF FF FF FF FF FF FF                 
0xB0:  FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF                 
0xC0:  FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF                 
0xD0:  FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF                 
0xE0:  FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF                 
0xF0:  FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 3E 

Done
```

If you see this, congratulations, you have successfully configured the Arduino IDE to work with the AllWize K2 board!

[Previous: Install the AllWize library](04-allwizek2-allwize-library.md) |
[Tutorial menu](readme.md)
