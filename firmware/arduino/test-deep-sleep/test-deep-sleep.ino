#include "Zanshin_BME680.h"  // Include the BME680 Sensor library

/* EPD Power Enable */
#define EPD_POWER_EN 22

/* BME688 Object */
BME680_Class BME680;
#define I2C_SDA 6
#define I2C_SCL 7

/* Soil Moisture */
#define SM_SENSE_OUT 4
#define SM_PWM_PIN 23

/* Battery Measurement */
#define BATT_MEAS 2
#define BATT_MEAS_EN 3

/* Deep-sleep test */
#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  30        /* Time ESP32 will go to sleep (in seconds) */

void setup()
{
  /* Initialise UART */
  Serial.begin(115200);
  delay(100);

  /* Disable Display */
  pinMode( EPD_POWER_EN, OUTPUT);
  digitalWrite( EPD_POWER_EN, LOW);

  /* Initialise and Disable BME688 */
  Wire.setPins(I2C_SDA, I2C_SCL);
  Serial.print(F("Starting I2CDemo example program for BME680\n"));
  Serial.print(F("- Initializing BME680 sensor\n"));
  while (!BME680.begin(I2C_STANDARD_MODE)) {  // Start BME680 using I2C, use first device found
    Serial.print(F("-  Unable to find BME680. Trying again in 5 seconds.\n"));
    delay(5000);
  }  // of loop until device is located
  Serial.print(F("- Setting 16x oversampling for all sensors\n"));
  BME680.setOversampling(TemperatureSensor, Oversample16);  // Use enumerated type values
  BME680.setOversampling(HumiditySensor, Oversample16);     // Use enumerated type values
  BME680.setOversampling(PressureSensor, Oversample16);     // Use enumerated type values
  Serial.print(F("- Setting IIR filter to a value of 4 samples\n"));
  BME680.setIIRFilter(IIR4);  // Use enumerated type values
  Serial.print(F("- Setting gas measurement to 320\xC2\xB0\x43 for 150ms\n"));  // "�C" symbols
  BME680.setGas(320, 150);  // 320�c for 150 milliseconds
  /* disable I2C pin */
  pinMode( I2C_SDA, INPUT );
  pinMode( I2C_SCL, INPUT );

  /* Disable Soil Moisture Sensor */
  pinMode( SM_SENSE_OUT, INPUT );
  pinMode( SM_PWM_PIN, OUTPUT );
  digitalWrite( SM_PWM_PIN, LOW );

  /* Disable Battery Measurement */
  pinMode( BATT_MEAS, INPUT );
  pinMode( BATT_MEAS_EN, OUTPUT );
  digitalWrite( BATT_MEAS_EN, LOW );

  /* Deep-sleep now */
  Serial.println("Going to sleep for " + String(TIME_TO_SLEEP) + " Seconds");
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.flush(); 
  esp_deep_sleep_start();
}

void loop(){ }