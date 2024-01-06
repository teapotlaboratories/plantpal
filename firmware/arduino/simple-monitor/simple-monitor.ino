#include "Zanshin_BME680.h"  // Include the BME680 Sensor library

#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

#include <StreamString.h>
#define PrintString StreamString

#include "plantpal_graphics.h"

// Plantpal
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

/* Soil Moisture */
#define SM_SENSE_OUT 22   // Rev 2 Board incorrectly wire analog in, use Pin 4 instead.
#define SM_SENSE_OUT_NEW 4  // use this pin instead
#define SM_PWM_PIN 23
#define SM_PWM_FREQ 500000
#define SM_PWM_RESOLUTION 3
#define SM_PWM_DUTY_CYCLE 4
#define SM_ADC_MAX  2657
#define SM_ADC_MIN  848
#define SM_ADC_STEP_3SEG  603

/* Battery Measurement */
#define BATT_MEAS 2
#define BATT_MEAS_EN 3
#define BATT_MEAS_VD_R1 4700.0  // 4.7K ohm
#define BATT_MEAS_VD_R2 10000.0 // 10K Ohm

void setup()
{
  /* Initialise UART */
  Serial.begin(115200);
  delay(100);

  /* Initialise Display */
  // initialize SPI
  fspi.begin(EPD_SCK, -1, EPD_SDI, -1); // SCLK, MISO, MOSI, SS
  display.epd2.selectSPI(fspi, SPISettings(4000000, MSBFIRST, SPI_MODE0));
  // initialize display
  display.init(0); // default 10ms reset pulse, e.g. for bare panels with DESPI-C02, set to 0 to disable display serial diag out
  // first update should be full refresh
  StartupDisplay();
  delay(1000);

  /* Initialise BME688 */
  Wire.setPins(6, 7);
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

  /* Initialise ADC Read Resolution */
  analogReadResolution(12);

  /* Initialise Soil Moisture Sensor */
  pinMode( SM_SENSE_OUT_NEW, INPUT );
  pinMode( SM_SENSE_OUT, INPUT );
  pinMode( SM_PWM_PIN, OUTPUT );
  digitalWrite( SM_PWM_PIN, LOW );

  /* Initialise Battery Measurement */
  pinMode( BATT_MEAS, INPUT );
  pinMode( BATT_MEAS_EN, OUTPUT );
  digitalWrite( BATT_MEAS_EN, LOW );
}

void loop()
{
  int32_t  temp, humidity, pressure, gas;  // BME readings
  float temp_f;
  static Face current_face = Face::kHappy;
  static Face target_face;

  /* max adc soil: 2657 
     min adc soil: 848 
     step size for 3 seg: 603 */
  while( 1 )
  {
    /* read bme688 */
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

    if( target_face != current_face )
    {
      current_face = target_face;
      DrawFacePartial(current_face);
    }

    WriteLeftColumn( temp_f, "c" );
    WriteRightColumn( batt, "v" );

    display.hibernate();
    // delay(500); 
    // delay(1000); 
    delay(300000); 
  }
}

// note for partial update window and setPartialWindow() method:
// partial update window size and position is on byte boundary in physical x direction
// the size is increased in setPartialWindow() if x or w are not multiple of 8 for even rotation, y or h for odd rotation
// see also comment in GxEPD2_BW.h, GxEPD2_3C.h or GxEPD2_GFX.h for method setPartialWindow()
void StartupDisplay()
{
  const char startup_text_r1[] = "Teapotlabs";
  const char startup_text_r2[] = "Plantpal";
  
  int16_t tbx, tby; 
  uint16_t tbw, tbh;

  display.setRotation(0);
  display.setFont(&FreeSans12pt7b);
  display.setTextColor(GxEPD_BLACK);

  display.getTextBounds(startup_text_r1, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t x1 = (display.width() - tbw) / 2;
  uint16_t y1 = ((display.height() / 3) - tbh / 2) - tby; // y is base line!
  display.getTextBounds(startup_text_r2, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t x2 = (display.width() - tbw) / 2;
  uint16_t y2 = ((display.height() / 2) - tbh / 2) - tby; // y is base line!

  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x1, y1);
    display.print(startup_text_r1);
    display.setCursor(x2, y2);
    display.print(startup_text_r2);
  }
  while (display.nextPage());
}

void DrawFacePartial( Face face )
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
 
  // display.writeScreenBuffer(); // use default for white
  display.writeImagePart(image, 0, 0, 200, 200,
                                0, 0, 200, 150, false, false, true);
  display.refresh(true);

  // for differential update: set previous buffer equal to current buffer in controller
  display.epd2.writeScreenBufferAgain(); // use default for white
  display.epd2.writeImagePartAgain(image, 0, 0, 200, 200,
                                          0, 0, 200, 150, false, false, true);
}

void WriteLeftColumn(float value, String unit)
{
  // use asymmetric values for test
  uint16_t box_x = 20;
  uint16_t box_y = 170;
  uint16_t box_w = 70;
  uint16_t box_h = 20;
  uint16_t cursor_y = box_y + box_h - 6;

  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  
  display.setPartialWindow(box_x, box_y, box_w, box_h);
  display.firstPage();
  do
  {
    display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
    display.setCursor(box_x, cursor_y);
    display.print(value, 1);
    display.print(unit);
  }
  while (display.nextPage());
}

void WriteRightColumn(float value, String unit)
{
  // use asymmetric values for test
  uint16_t box_x = 120;
  uint16_t box_y = 170;
  uint16_t box_w = 70;
  uint16_t box_h = 20;
  uint16_t cursor_y = box_y + box_h - 6;

  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  
  display.setPartialWindow(box_x, box_y, box_w, box_h);
  display.firstPage();
  do
  {
    display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
    display.setCursor(box_x, cursor_y);
    display.print(value, 2);
    display.print(unit);
  }
  while (display.nextPage());
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
    sum += analogRead( SM_SENSE_OUT_NEW );
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