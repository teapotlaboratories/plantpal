#include <WiFi.h>
#include <ArduinoMqttClient.h>

/* define deep-sleep time */
#define SLEEP_TIME_S   30 // 5 minutes
#define US_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */

// WiFi Credential
const char* ssid     = "YOUR SSID";
const char* password = "YOUR SSID PASSWORD";

// connection object
WiFiClient client;
MqttClient mqttClient( client );

// MQTT Broker server
const char broker[] = "test.mosquitto.org";
int        port     = 1883;
const char topic[]  = "plantpal/test";

void setup()
{
    // We start by connecting to a WiFi network
    // To debug, please enable Core Debug Level to Verbose
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
            break;
          case WL_CONNECT_FAILED:
            return;
            break;
          case WL_CONNECTION_LOST:
            break;
          case WL_SCAN_COMPLETED:
            break;
          case WL_DISCONNECTED:
            break;
          case WL_CONNECTED:
            // connect to MQTT Broker
            if ( !mqttClient.connect(broker, port) ) {
              while (1) delay( 1000 );
            }

            sendMqttMessage();
            return;
            break;
          default:
            break;
        }
        delay(tryDelay);
        
        if(numberOfTries <= 0){
          // Use disconnect function to force stop trying to connect
          WiFi.disconnect();
          return;
        } else {
          numberOfTries--;
        }
    }
}

void loop(){}

void sendMqttMessage() {

    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(topic);
    mqttClient.print("hello");
    mqttClient.endMessage();
    
    // give time for MQTT to finish publish message
    delay( 400 );

    /* deepsleep now */
    esp_sleep_enable_timer_wakeup( SLEEP_TIME_S * US_TO_S_FACTOR ); // sleep for 15 seconds
    esp_deep_sleep_start();
    // mcu goes to sleep and restart on wake-up

}