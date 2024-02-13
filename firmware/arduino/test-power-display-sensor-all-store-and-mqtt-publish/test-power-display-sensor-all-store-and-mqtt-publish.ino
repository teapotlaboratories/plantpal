#include <WiFi.h>
#include <ArduinoMqttClient.h>

#include "Zanshin_BME680.h"  // Include the BME680 Sensor library

#include <GxEPD2_BW.h>
#include "plantpal_graphics.h"

/* write to storage in internal flash */
#include "FS.h"
#include "SPIFFS.h"
#define FORMAT_SPIFFS_IF_FAILED true

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

/* define deep-sleep time */
#define SLEEP_TIME_S   30 // 5 minutes
#define US_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */

// EPD Pins
#define EPD_POWER_EN 22
#define EPD_CS 19
#define EPD_DC 18
#define EPD_RST 15
#define EPD_BUSY 14
#define EPD_SDI 21
#define EPD_SCK 20

#define MAX_DISPLAY_BUFFER_SIZE 65536ul // e.g.
#define MAX_DISPLAY_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))
SPIClass fspi(FSPI);
GxEPD2_BW< GxEPD2_154_GDEY0154D67, 
           MAX_DISPLAY_HEIGHT(GxEPD2_154_GDEY0154D67) > display( GxEPD2_154_GDEY0154D67 ( EPD_CS, 
                                                                                          EPD_DC, 
                                                                                          EPD_RST, 
                                                                                          EPD_BUSY) ); // adapt the constructor parameters to your wiring

/* BME688 Object */
BME680_Class BME680;
#define I2C_SDA 6
#define I2C_SCL 7

/* Soil Moisture Pins */
#define SM_SENSE_OUT 4
#define SM_PWM_PIN 23
#define SM_PWM_FREQ 500000
#define SM_PWM_RESOLUTION 3
#define SM_PWM_DUTY_CYCLE 4
#define SM_ADC_MAX  2657
#define SM_ADC_MIN  848
#define SM_ADC_STEP_3SEG  603

/* Battery Measurement Pins */
#define BATT_MEAS 2
#define BATT_MEAS_EN 3
#define BATT_MEAS_VD_R1 4700.0  // 4.7K ohm
#define BATT_MEAS_VD_R2 10000.0 // 10K Ohm

void setup()
{
  /* We start by connecting to a WiFi network */
  WiFi.begin(ssid, password); // system will try to connect while we do something else
  // To debug, please enable Core Debug Level to Verbose
  // Auto reconnect is set true as default
  // To set auto connect off, use the following function
  // WiFi.setAutoReconnect(false);

  /* Initialise SPIFFS Storage */
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
      return;
  }
  
  /* Initialise Display */
  // enable EPD power
  pinMode( EPD_POWER_EN, OUTPUT);
  digitalWrite( EPD_POWER_EN, HIGH);
  // initialize SPI
  fspi.begin(EPD_SCK, -1, EPD_SDI, -1); // SCLK, MISO, MOSI, SS
  display.epd2.selectSPI(fspi, SPISettings(4000000, MSBFIRST, SPI_MODE0));
  // initialize display
  display.init(0); // default 10ms reset pulse, e.g. for bare panels with DESPI-C02, set to 0 to disable display serial diag out

  /* Initialise and Disable BME688 */
  Wire.setPins(I2C_SDA, I2C_SCL);
  while (!BME680.begin(I2C_STANDARD_MODE)) {  // Start BME680 using I2C, use first device found
    delay(5000);
  }  // of loop until device is located
  BME680.setOversampling(TemperatureSensor, Oversample16);  // Use enumerated type values
  BME680.setOversampling(HumiditySensor, Oversample16);     // Use enumerated type values
  BME680.setOversampling(PressureSensor, Oversample16);     // Use enumerated type values
  BME680.setIIRFilter(IIR4);  // Use enumerated type values
  BME680.setGas(320, 150);  // 320�c for 150 milliseconds

  /* Initialise ADC Read Resolution */
  analogReadResolution(12);

  /* Initialise Soil Moisture Sensor */
  pinMode( SM_SENSE_OUT, INPUT );
  pinMode( SM_PWM_PIN, INPUT );

  /* Initialise Battery Measurement */
  pinMode( BATT_MEAS, INPUT );
  pinMode( BATT_MEAS_EN, OUTPUT );
  digitalWrite( BATT_MEAS_EN, LOW );

  /*********** main program ***********/
  int32_t  temp, humidity, pressure, gas;  // BME readings
  float temp_f;
  static Face target_face;

  /* read bme688 */
  BME680.getSensorData(temp, humidity, pressure, gas); // ignore first readings, might be incorrect
  delay(100);
  BME680.getSensorData(temp, humidity, pressure, gas);
  temp_f = temp / 100.0;
  
  /* read battery voltage */
  float batt = ReadBatteryMillivolts() / 1000.0;

  /* read soil moisutre */
  uint16_t soil_adc = ReadSoilMoistureAdcValue();
  if( soil_adc >= 1480 )
  {
    target_face = Face::kSad;
  } 
  else if( 1480 > soil_adc && 1000 <= soil_adc )
  {
    target_face = Face::kMeh ;
  }
  else if( 900 > soil_adc )
  {
    target_face = Face::kHappy ;
  }

  /* draw planpal face */
  DrawFaceFull(target_face);
  display.hibernate();
  digitalWrite( EPD_POWER_EN, LOW ); // power-off EPD

  /* store measurement in storage */
  appendFile(SPIFFS, "/measurement.txt", "sample_data");  

  /* Assume wifi is connected */
  // Will try for about 10 seconds (20x 500ms)
  int tryDelay = 500;
  int numberOfTries = 20;
  // Check current wifi state
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

          sendMqttMessage( SPIFFS, "/measurement.txt" );
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

void sendMqttMessage( fs::FS &fs, const char * path ) {

    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(topic);
    // get message from internal storage
    File file = fs.open(path);
    if(!file || file.isDirectory()){
        // failed to open file for reading
        return;
    }
    while(file.available()){
        mqttClient.write(file.read());
    }
    file.close();
    
    mqttClient.endMessage();

    // delete after successfully send
    deleteFile( fs, path );
    
    // give time for MQTT to finish publish message
    delay( 400 );

    /* deepsleep now */
    esp_sleep_enable_timer_wakeup( SLEEP_TIME_S * US_TO_S_FACTOR ); // sleep for 15 seconds
    esp_deep_sleep_start();
    // mcu goes to sleep and restart on wake-up

}

void loop(){}

void DrawFaceFull( Face face )
{
  const unsigned char* image;
  
  switch( face )
  {
    case Face::kHappy:
    {
      image = happy_bg;
      break;
    }
    case Face::kMeh:
    {
      image = meh_bg;
      break;
    }
    case Face::kSad:
    {
      image = sad_bg;
      break;
    }
    default:
    {
      return;
    }
  }
 
  display.setFullWindow();
  
  bool m = display.mirror(false);
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.drawInvertedBitmap(0, 0, image, 200, 200, GxEPD_BLACK);
  }
  while (display.nextPage());
  display.mirror(m);
}

uint16_t ReadSoilMoistureAdcValue()
{
  /* enable pwm pin */
  ledcAttach( SM_PWM_PIN, SM_PWM_FREQ, SM_PWM_RESOLUTION );
  ledcWrite( SM_PWM_PIN, SM_PWM_DUTY_CYCLE );

  /* let pwm generate pulses before we read */
  delay(10);

  /* read soil moisture */
  const int n = 1;
  int sum = 0;
  for (int i = 0; i < n; i++) {
    sum += analogRead( SM_SENSE_OUT );
  }
  int ret = sum / n;

  /* disable pwm pin */
  ledcDetach( SM_PWM_PIN );
  pinMode( SM_PWM_PIN, INPUT );
  return ret;
}

uint16_t ReadBatteryMillivolts()
{ 
  digitalWrite( BATT_MEAS_EN, HIGH );
  delay(20); // wait until voltage is stable
  uint16_t batt_vd = analogReadMilliVolts( BATT_MEAS );
  digitalWrite( BATT_MEAS_EN, LOW );
  
  // calculate battery voltage based 
  // on voltage divider resistor
  // Vsource = Vout (R1 + R2) / ( R2 ) 
  return ( batt_vd * (BATT_MEAS_VD_R1 + BATT_MEAS_VD_R2) ) / BATT_MEAS_VD_R2 ;
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    // Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        // Serial.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        // Serial.println("- message appended");
    } else {
        // Serial.println("- append failed");
    }
    file.close();
}

void deleteFile(fs::FS &fs, const char * path){
    // Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        // Serial.println("- file deleted");
    } else {
        // Serial.println("- delete failed");
    }
}