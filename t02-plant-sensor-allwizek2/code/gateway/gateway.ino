/*

AllWize - WIZE 2 MyDevices Bridge

Listens to messages on the same channel, data rate and CF and
forwards them to MyDevice via an MQTT broker.

This example is meant to run on a Wemos D1 board (ESP8266).

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

// -----------------------------------------------------------------------------
// Dependencies
// -----------------------------------------------------------------------------

#if not defined(ARDUINO_ARCH_ESP8266)
    #error "This example is meant to run on an ESP8266 board!"
#endif

#include "AllWize.h"
#include "SoftwareSerial.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>
#include "CayenneLPP.h"
#include "ArduinoJson.h"
#include "credentials.h"

//------------------------------------------------------------------------------
// Configuration
//------------------------------------------------------------------------------

#define DEBUG_SERIAL            Serial

#define RESET_PIN               14
#define RX_PIN                  5
#define TX_PIN                  4

#define WIZE_CHANNEL            CHANNEL_04
#define WIZE_DATARATE           DATARATE_2400bps

#define CAYENNE_HOST            "mqtt.mydevices.com"
#define CAYENNE_PORT            1883

// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------

WiFiClient wifi_client;
void mqttOnMessage(char* topic, byte* payload, unsigned int length);
PubSubClient mqtt(CAYENNE_HOST, CAYENNE_PORT, mqttOnMessage, wifi_client);
Ticker mqttTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiTimer;

AllWize allwize(RX_PIN, TX_PIN, RESET_PIN);

// -----------------------------------------------------------------------------
// MQTT
// -----------------------------------------------------------------------------

void mqttOnMessage(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

void mqttConnect() {
    
    if (!wifiConnected()) return;
    
    DEBUG_SERIAL.println("[MQTT] Connecting...");
    
    // Attempt to connect
    if (mqtt.connect(CAYENNE_CLIENTID, CAYENNE_USERNAME, CAYENNE_PASSWORD)) {
      DEBUG_SERIAL.println("[MQTT] Connected!");

    } else {
      DEBUG_SERIAL.print("[MQTT] Connection failed, rc=");
      DEBUG_SERIAL.println(mqtt.state());
      delay(5000);
    }
        
}

void mqttSend(const char * topic, const char * payload) {

    char buffer[128];
    snprintf(buffer, sizeof(buffer), "[MQTT] Sending: %s => %s\n", topic, payload);
    DEBUG_SERIAL.print(buffer);

    mqtt.publish(topic, payload);

}

// -----------------------------------------------------------------------------
// Cayenne
// -----------------------------------------------------------------------------

const char * cayenneUnit(uint8_t type) {
    if (0 == type) return "db";
    if (LPP_DIGITAL_INPUT == type) return "d";
    if (LPP_DIGITAL_OUTPUT == type) return "d";
    if (LPP_LUMINOSITY == type) return "lux";
    if (LPP_PRESENCE == type) return "d";
    if (LPP_TEMPERATURE == type) return "c";
    if (LPP_RELATIVE_HUMIDITY == type) return "p";
    if (LPP_ACCELEROMETER == type) return "g";
    if (LPP_BAROMETRIC_PRESSURE == type) return "hpa";
    if (LPP_VOLTAGE == type) return "v";
    if (LPP_CURRENT == type) return "a";
    if (LPP_FREQUENCY == type) return "hz";
    if (LPP_PERCENTAGE == type) return "p";
    if (LPP_ALTITUDE == type) return "m";
    if (LPP_POWER == type) return "w";
    if (LPP_DISTANCE == type) return "m";
    if (LPP_ENERGY == type) return "kwh";
    if (LPP_DIRECTION == type) return "deg";
    if (LPP_UNIXTIME == type) return "s";
    if (LPP_GYROMETER == type) return "dps";
    if (LPP_GPS == type) return "m";
    if (LPP_SWITCH == type) return "d";
    return "";
}

void cayenneSend(uint8_t channel, const char * name, uint8_t type, float value) {

    char topic[128];
    snprintf(
        topic, sizeof(topic), 
        "v1/%s/things/%s/data/%d",
        CAYENNE_USERNAME, CAYENNE_CLIENTID, channel
    );

    char payload[64];
    snprintf(
        payload, sizeof(payload),
        "%s,%s=%.2f",
        name, cayenneUnit(type), value
    );
    
    mqttSend(topic, payload);

}

// -----------------------------------------------------------------------------
// Wize
// -----------------------------------------------------------------------------

void wizeSetup() {

    // Init AllWize object
    allwize.begin();
    if (!allwize.waitForReady()) {
        DEBUG_SERIAL.println("[WIZE] Error connecting to the module, check your wiring!");
        while (true) delay(1);
    }

    allwize.master();
    allwize.setChannel(WIZE_CHANNEL, true);
    allwize.setDataRate(WIZE_DATARATE);

    DEBUG_SERIAL.print("[WIZE] Module type: "); DEBUG_SERIAL.println(allwize.getModuleTypeName());
    DEBUG_SERIAL.print("[WIZE] Channel: "); DEBUG_SERIAL.println(allwize.getChannel());
    DEBUG_SERIAL.print("[WIZE] Datarate: "); DEBUG_SERIAL.println(allwize.getDataRate());
    DEBUG_SERIAL.print("[WIZE] Device UID: "); DEBUG_SERIAL.println(allwize.getUID());
    DEBUG_SERIAL.println("[WIZE] Ready...");

}

void wizeDebug(allwize_message_t message) {

    // Code to pretty-print the message
    char buffer[128];
    snprintf(
        buffer, sizeof(buffer),
        "[WIZE] ADDR: 0x%02X%02X%02X%02X, RSSI: %d, DATA: ",
        message.address[0], message.address[1],
        message.address[2], message.address[3],
        (int16_t) message.rssi / -2
    );
    DEBUG_SERIAL.print(buffer);

    for (uint8_t i=0; i<message.len; i++) {
        snprintf(buffer, sizeof(buffer), "%02X", message.data[i]);
        DEBUG_SERIAL.print(buffer);
    }
    DEBUG_SERIAL.println();

}

void wizeParse(allwize_message_t message) {

    char uid[10];
    snprintf(
        uid, sizeof(uid), "%02X%02X%02X%02X",
        message.address[0], message.address[1],
        message.address[2], message.address[3]
    );

    // RSSI
    cayenneSend(0, "rssi", 0, message.rssi / -2.0);

    // Parse payload
    CayenneLPP payload(1);
    DynamicJsonDocument jsonBuffer(512);
    JsonArray root = jsonBuffer.createNestedArray();
    payload.decode(message.data, message.len, root);

    // Walk JsonArray
    for (JsonObject element: root) {
        JsonVariant v = element["value"];
        if (v.is<JsonObject>()) {
            for (JsonPair kv : v.as<JsonObject>()) {

                cayenneSend(
                    element["channel"].as<uint8_t>(), 
                    kv.key().c_str(),
                    element["type"].as<uint8_t>(),
                    kv.value().as<float>()
                );

            }
        } else {

            cayenneSend(
                element["channel"].as<uint8_t>(), 
                element["name"].as<char*>(),
                element["type"].as<uint8_t>(),
                element["value"].as<float>()
            );

        }
    }

}

void wizeLoop() {

    if (allwize.available()) {

        // Get the message
        allwize_message_t message = allwize.read();

        // Show it to console
        wizeDebug(message);

        // Parse and send via MQTT
        if (mqtt.connected()) wizeParse(message);

    }

}

// -----------------------------------------------------------------------------
// WiFi
// -----------------------------------------------------------------------------

bool wifiConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void wifiConnect() {
    DEBUG_SERIAL.println("[WIFI] Connecting...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void wifiOnConnect(const WiFiEventStationModeGotIP& event) {
    DEBUG_SERIAL.println("[WIFI] Connected!");
}

void wifiOnDisconnect(const WiFiEventStationModeDisconnected& event) {
    DEBUG_SERIAL.println("[WIFI] Disconnected!");
    wifiTimer.detach();
    wifiTimer.once(2, wifiConnect);
}

void wifiSetup() {
    wifiConnectHandler = WiFi.onStationModeGotIP(wifiOnConnect);
    wifiDisconnectHandler = WiFi.onStationModeDisconnected(wifiOnDisconnect);
}

// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------

void setup() {

    // Setup serial DEBUG_SERIAL
    DEBUG_SERIAL.begin(115200);
    while (!DEBUG_SERIAL && millis() < 5000);
    DEBUG_SERIAL.println();
    DEBUG_SERIAL.println("[MAIN] Wize 2 MyDevice bridge");
    DEBUG_SERIAL.println();

    //wizeSetup();
    wifiSetup();
    
    wifiConnect();

}

void loop() {

    // Connect to MQTT
    if (!mqtt.connected()) mqttConnect();
    mqtt.loop();

    // Listen to messages
    //wizeLoop();

    static unsigned long last = 0;
    if (millis() - last < 20000) return;
    last = millis();
    cayenneSend(0, "rssi", 0, 25);


}
