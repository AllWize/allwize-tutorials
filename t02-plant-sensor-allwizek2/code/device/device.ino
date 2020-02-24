/*

AllWize - Plant sensor - AllWize K2

Simple slave that sends faked data using CayenneLPP frame format.

Copyright (C) 2020 by AllWize <github@allwize.io>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <AllWize.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <CayenneLPP.h>

// -----------------------------------------------------------------------------
// Board configuration
// -----------------------------------------------------------------------------

// Check http://wiki.allwize.io/index.php?title=Allwize_K2#Arduino_IDE
// to add support to Allwize K2 board in Arduino IDE
#if not defined(ARDUINO_ALLWIZE_K2)
    #error "This example is meant to run on an AllWize K2 board!"
#endif

#define MODULE_SERIAL           SerialWize
#define DEBUG_SERIAL            SerialUSB
#define RESET_PIN               PIN_WIZE_RESET

// -----------------------------------------------------------------------------
// Configuration
// -----------------------------------------------------------------------------

#define WIZE_CHANNEL            CHANNEL_04
#define WIZE_POWER              POWER_20dBm
#define WIZE_DATARATE           DATARATE_2400bps
#define WIZE_UID                0x20212223

#define SLEEP_TIME              20000    // in ms

#define HIGROMETER_PIN          A1
#define SAMPLE_COUNT            10
#define HIGROMETER_MIN          300     // Value when in water
#define ANALOG_DEPTH            12
#define ANALOG_MAX              ((1 << ANALOG_DEPTH) - 1)

// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------

AllWize allwize(&MODULE_SERIAL, RESET_PIN);
Adafruit_BME280 sensor;
CayenneLPP payload(32);

// -----------------------------------------------------------------------------
// AllWize
// -----------------------------------------------------------------------------

void wizeSetup() {

    DEBUG_SERIAL.println("Initializing radio module");

    // Init AllWize object
    allwize.begin();
    if (!allwize.waitForReady()) {
        DEBUG_SERIAL.println("[WIZE] Error connecting to the module, check your wiring!");
        while (true);
    }

    allwize.slave();
    allwize.setChannel(WIZE_CHANNEL, true);
    allwize.setPower(WIZE_POWER);
    allwize.setDataRate(WIZE_DATARATE);
    allwize.setUID(WIZE_UID);

    DEBUG_SERIAL.print("[WIZE] Module type: "); DEBUG_SERIAL.println(allwize.getModuleTypeName());
    DEBUG_SERIAL.print("[WIZE] Channel: "); DEBUG_SERIAL.println(allwize.getChannel());
    DEBUG_SERIAL.print("[WIZE] Datarate: "); DEBUG_SERIAL.println(allwize.getDataRate());
    DEBUG_SERIAL.print("[WIZE] Device UID: "); DEBUG_SERIAL.println(allwize.getUID());
    DEBUG_SERIAL.println("[WIZE] Ready...");

}

void wizeSend(uint8_t * payload, size_t len) {

    char buffer[64];
    DEBUG_SERIAL.print("[WIZE] Sending: ");
    for (uint8_t i = 0; i<len; i++) {
        snprintf(buffer, sizeof(buffer), "%02X", payload[i]);
        DEBUG_SERIAL.print(buffer);
    }
    DEBUG_SERIAL.print("\n");

    if (!allwize.send(payload, len)) {
        DEBUG_SERIAL.println("[WIZE] Error sending message");
    }

}

// -----------------------------------------------------------------------------
// Dummy sensors
// -----------------------------------------------------------------------------

void sensorSetup() {

    // Init moisture sensor
    pinMode(ANALOG_DEPTH, INPUT);
    analogReadResolution(ANALOG_DEPTH);

    // Init BME280 sensor
    if (!sensor.begin(0x76)) {
        DEBUG_SERIAL.println("[SENSOR] Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
    
    DEBUG_SERIAL.println("[SENSOR] Sensors ready!");    

}

// Returns the temperature in C
float getTemperature() {
    return sensor.readTemperature();
}

// Returns the himidity in %
float getHumidity() {
    return (float) sensor.readHumidity();
}

// Returns the pressure in hPa
float getPressure() {
    return (float) sensor.readPressure();
}

// Returns the soil moisture in %
float getMoisture() {

    double value = 0;
    for (unsigned char i=0; i<SAMPLE_COUNT; i++) {
        value += analogRead(HIGROMETER_PIN);
    }
    value = value / SAMPLE_COUNT;

    // Uncomment this line to get a reading of the sensor immersed in water,
    // set HIGROMETER_MIN to this value
    //DEBUG_SERIAL.print("Sensor reading: "); DEBUG_SERIAL.println(value);

    value = map(value, HIGROMETER_MIN, ANALOG_MAX, 100, 0);
    return constrain(value, 0, 100);
   
}

// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------

void sleep(uint32_t ms) {
    // Actually we should sleep the board here :)
    delay(ms);
}

void setup() {

    // Init serial DEBUG_SERIAL
    DEBUG_SERIAL.begin(115200);
    while (!DEBUG_SERIAL && millis() < 5000);
    DEBUG_SERIAL.println();
    DEBUG_SERIAL.println("[MAIN] Plant sensor example");

    // Init radio
    wizeSetup();

    // Init sensors
    sensorSetup();

}

void loop() {

    // Build payload
    payload.reset();
    payload.addTemperature(1, getTemperature());
    payload.addRelativeHumidity(2, getHumidity());
    payload.addBarometricPressure(3, getPressure());
    payload.addPercentage(4, getMoisture());

    // Send the string as payload
    wizeSend(payload.getBuffer(), payload.getSize());

    // Delay responses
    sleep(SLEEP_TIME);

}