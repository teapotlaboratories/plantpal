/* wifi-mqtt-simple-publish

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
   
*/
#include <WiFi.h>
#include <ArduinoMqttClient.h>

// WiFi Credential
const char* ssid     = "YOUR WIFI SSID";
const char* password = "YOUR WIFI PASSWORD";

// connection object
WiFiClient client;
MqttClient mqttClient( client );

// MQTT Broker server
const char broker[] = "test.mosquitto.org";
int        port     = 1883;
const char topic[]  = "plantpal/test";

// variables for sending delay without delay()
const long interval = 1000;
unsigned long previousMillis = 0;
int count = 0;

void setup()
{
    Serial.begin(115200);

    // wait for awhile until usb serial fully connected
    delay(2000);

    // We start by connecting to a WiFi network
    // To debug, please enable Core Debug Level to Verbose
    Serial.println();
    Serial.print("[WiFi] Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);
    // Auto reconnect is set true as default
    // To set auto connect off, use the following function
    // WiFi.setAutoReconnect(false);

    // Will try for about 10 seconds (20x 500ms)
    int tryDelay = 500;
    int numberOfTries = 20;

    // Wait for the WiFi event
    while (true) {
        
        switch(WiFi.status()) {
          case WL_NO_SSID_AVAIL:
            Serial.println("[WiFi] SSID not found");
            break;
          case WL_CONNECT_FAILED:
            Serial.print("[WiFi] Failed - WiFi not connected! Reason: ");
            return;
            break;
          case WL_CONNECTION_LOST:
            Serial.println("[WiFi] Connection was lost");
            break;
          case WL_SCAN_COMPLETED:
            Serial.println("[WiFi] Scan is completed");
            break;
          case WL_DISCONNECTED:
            Serial.println("[WiFi] WiFi is disconnected");
            break;
          case WL_CONNECTED:
            Serial.println("[WiFi] WiFi is connected!");
            Serial.print("[WiFi] IP address: ");
            Serial.println(WiFi.localIP());
            
            // connect to MQTT Broker
            if ( !mqttClient.connect(broker, port) ) {
              Serial.print("MQTT connection failed! Error code = ");
              Serial.println(mqttClient.connectError());
              while (1) delay( 1000 );
            }
            Serial.println("You're connected to the MQTT broker!");
            Serial.println();
            return;

            break;
          default:
            Serial.print("[WiFi] WiFi Status: ");
            Serial.println(WiFi.status());
            break;
        }
        delay(tryDelay);
        
        if(numberOfTries <= 0){
          Serial.print("[WiFi] Failed to connect to WiFi!");
          // Use disconnect function to force stop trying to connect
          WiFi.disconnect();
          return;
        } else {
          numberOfTries--;
        }
    }
}


void loop() {
    // call poll() regularly to allow the library to send MQTT keep alives which
    // avoids being disconnected by the broker
    mqttClient.poll();

    // to avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay
    // see: File -> Examples -> 02.Digital -> BlinkWithoutDelay for more info
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
        // save the last time a message was sent
        previousMillis = currentMillis;

        Serial.print("Sending message to topic: ");
        Serial.println(topic);
        Serial.print("hello ");
        Serial.println(count);

        // send message, the Print interface can be used to set the message contents
        mqttClient.beginMessage(topic);
        mqttClient.print("hello ");
        mqttClient.print(count);
        mqttClient.endMessage();

        Serial.println();

        count++;
    }
}