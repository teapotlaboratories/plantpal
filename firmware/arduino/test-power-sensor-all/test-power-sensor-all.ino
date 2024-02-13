#include "Zanshin_BME680.h"  // Include the BME680 Sensor library

#include <GxEPD2_BW.h>
#include "plantpal_graphics.h"

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
  /* Disable Display */
  pinMode( EPD_POWER_EN, OUTPUT);
  digitalWrite( EPD_POWER_EN, LOW);

  /* Initialise BME688 */
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

  /* all sensor reading have not been calibrated and just
     a way to give a simple example */  

  /* read bme688 */
  BME680.getSensorData(temp, humidity, pressure, gas); // ignore first readings, might be incorrect
  delay(100);
  BME680.getSensorData(temp, humidity, pressure, gas);
  temp_f = temp / 100.0;

  /* read battery voltage */
  float batt = ReadBatteryMillivolts() / 1000.0;

  /* read soil moisutre */
  uint16_t soil_adc = ReadSoilMoistureAdcValue();
  
  /* deepsleep now */
  esp_sleep_enable_timer_wakeup( SLEEP_TIME_S * US_TO_S_FACTOR ); // sleep for 15 seconds
  esp_deep_sleep_start();
  // mcu goes to sleep and restart on wake-up

}

void loop(){}

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