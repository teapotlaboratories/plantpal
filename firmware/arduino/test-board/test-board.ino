#include "Zanshin_BME680.h"  // Include the BME680 Sensor library

#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

#include <StreamString.h>
#define PrintString StreamString

#include "plantpal_graphics.h"

/* EPD Wiring */
#define EPD_CS 19
#define EPD_DC 18
#define EPD_RST 15
#define EPD_BUSY 14
#define EPD_SDI 21
#define EPD_SCK 20
#define EPD_POWER_EN 22

#define MAX_DISPLAY_BUFFER_SIZE 65536ul // e.g.
#define MAX_DISPLAY_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))
SPIClass fspi(FSPI);
GxEPD2_BW< GxEPD2_154_GDEY0154D67, 
           MAX_DISPLAY_HEIGHT(GxEPD2_154_GDEY0154D67) > display( GxEPD2_154_GDEY0154D67 ( EPD_CS, 
                                                                                          EPD_DC, 
                                                                                          EPD_RST, 
                                                                                          EPD_BUSY) ); // adapt the constructor parameters to your wiring
// full refresh every (FULL_REFRESH_PERIOD_MS) ms
#define FULL_REFRESH_PERIOD_MS 300 * 1000

/* BME688 Object */
BME680_Class BME680;

/* Soil Moisture */
#define SM_SENSE_OUT 4
#define SM_PWM_PIN 23
#define SM_PWM_FREQ 500000
#define SM_PWM_RESOLUTION 3
#define SM_PWM_DUTY_CYCLE 4

/* Battery Measurement */
#define BATT_MEAS_PIN 2
#define BATT_MEAS_EN_PIN 3
#define BATT_MEAS_VD_R1 4700.0  // 4.7K ohm
#define BATT_MEAS_VD_R2 10000.0 // 10K Ohm

/* AEM10941 Pin Status */
#define MPP_STATUS_PIN 5
#define PWR_STATUS_PIN 8

/* User button */
#define USER_BUTTON_PIN 9

/* Deep-sleep test */
// #define TEST_DEEP_SLEEP
#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  15        /* Time ESP32 will go to sleep (in seconds) */

/* variable to catch interrupt events */
static uint32_t total_event_pwr_status;
static uint32_t total_event_mpp_status;
static uint32_t total_event_user_button;

void setup()
{
  /* Initialise UART */
  Serial.begin(115200);
  delay(100);

  /* Initialise Display */
  // power on EPD
  pinMode( EPD_POWER_EN, OUTPUT);
  digitalWrite( EPD_POWER_EN, HIGH);
  // initialize SPI
  fspi.begin(EPD_SCK, -1, EPD_SDI, -1); // SCLK, MISO, MOSI, SS
  display.epd2.selectSPI(fspi, SPISettings(4000000, MSBFIRST, SPI_MODE0));
  // initialize display
  display.init(0); // default 10ms reset pulse, e.g. for bare panels with DESPI-C02, set to 0 to disable display serial diag out
  // first update should be full refresh
  StartupDisplay();
  delay(1000);
  DrawHappyFace();

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
  pinMode( SM_SENSE_OUT, INPUT );
  pinMode( SM_PWM_PIN, OUTPUT );
  digitalWrite( SM_PWM_PIN, LOW );

  /* Initialise Battery Measurement */
  pinMode( BATT_MEAS_PIN, INPUT );
  pinMode( BATT_MEAS_EN_PIN, OUTPUT );
  digitalWrite( BATT_MEAS_EN_PIN, LOW );

  /* Initialise AEM10941 Status Pin */
  pinMode( MPP_STATUS_PIN, INPUT );
  pinMode( PWR_STATUS_PIN, INPUT );

  /* Initialise User Button */
  pinMode( USER_BUTTON_PIN, INPUT );

  /* Setup interrupt for counting AEM pin status and user button */
  attachInterrupt(digitalPinToInterrupt(PWR_STATUS_PIN), process_power_status, FALLING);
  attachInterrupt(digitalPinToInterrupt(MPP_STATUS_PIN), process_mpp_status, FALLING);
  attachInterrupt(digitalPinToInterrupt(USER_BUTTON_PIN), process_user_button, FALLING);

  /* Deep-sleep now */
#ifdef TEST_DEEP_SLEEP
  Serial.println("Going to sleep for " + String(TIME_TO_SLEEP) + " Seconds");
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.flush(); 
  esp_deep_sleep_start();
#endif

  /* clear scren to white background */
  FillScreen( GxEPD_WHITE );
}

void process_power_status() {
  total_event_pwr_status = ++total_event_pwr_status > 99999 ? 0 : total_event_pwr_status;
}
void process_mpp_status() {
  total_event_mpp_status = ++total_event_mpp_status > 9999 ? 0 : total_event_mpp_status;
}
void process_user_button() {
  total_event_user_button = ++total_event_user_button > 999 ? 0 : total_event_user_button;
}

void loop()
{
  ReadAllSensors();
}

void FillScreen(uint16_t colour)
{
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(colour);
  }
  while (display.nextPage());
  display.hibernate();
}

void WriteToDisplay( int16_t x, int16_t y, String text )
{
  int16_t box_x, box_y;
  uint16_t box_w, box_h;
  display.getTextBounds(text, x, y, &box_x, &box_y, &box_w, &box_h);

  // adjust to plantpal screen 
  box_x += 11;
  box_y += 20;

  // too lazy to the understand the library, but text will 
  // be cut if the following line is not added 
  box_h += 6;
  uint16_t cursor_y = box_y + box_h - 6;

  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  
  display.setPartialWindow(box_x, box_y, box_w, box_h);
  display.firstPage();
  do
  {
    display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
    display.setCursor(box_x, cursor_y);
    display.print(text);
  }
  while (display.nextPage());
}

uint8_t ReadMPPStatus()
{
  // Asserted when the AEM performs a MPP evaluation.

  // Assert signal is inverse, caused by the 
  // MPP status driver ( see schematics )
  return !digitalRead( MPP_STATUS_PIN );
}

uint8_t ReadPowerStatus()
{
  // Asserted during 600 ms if the battery voltage falls below VOVDIS\

  // Assert signal is inverse, caused by the 
  // power status driver ( see schematics )
  return !digitalRead( PWR_STATUS_PIN );
}

uint8_t ReadUserButton()
{
  // Note:  User button is also shared with ESP32
  //        BOOT1 pin

  // User button signal is inverse because it is externally pulled-up.
  return !digitalRead( USER_BUTTON_PIN );
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
  digitalWrite( BATT_MEAS_EN_PIN, HIGH );
  delay(20); // wait until voltage is stable
  uint16_t batt_vd = analogReadMilliVolts( BATT_MEAS_PIN );
  digitalWrite( BATT_MEAS_EN_PIN, LOW );
  
  // calculate battery voltage based 
  // on voltage divider resistor
  // Vsource = Vout (R1 + R2) / ( R2 ) 
  return ( batt_vd * (BATT_MEAS_VD_R1 + BATT_MEAS_VD_R2) ) / BATT_MEAS_VD_R2 ;
}

void ReadAllSensors()
{  
  static int32_t  temp, humidity, pressure, gas;  // BME readings
  static char     buf[16];                        // sprintf text buffer
  static float    alt;                            // Temporary variable
  static uint16_t loopCounter = 0;                // Display iterations
  static uint16_t sm_adc;
  static uint16_t batt;
  static uint8_t mpp_status;
  static uint8_t pwr_status;
  static uint8_t usr_button;

  if (loopCounter % 25 == 0) {                    // Show header @25 loops
    Serial.print(F("\nLoop Temp\xC2\xB0\x43 Humid% Press hPa Air m\xE2\x84\xA6    SoilMstr  Batt mV"));
    Serial.print(F("\n==== ====== ====== ========= ========= ========= =========\n"));  // "�C" symbol
  }                                                     // if-then time to show headers
  // Get sensor data
  BME680.getSensorData(temp, humidity, pressure, gas);  // Get readings
  sm_adc = ReadSoilMoistureAdcValue();
  batt = ReadBatteryMillivolts();
  mpp_status = ReadMPPStatus();
  pwr_status = ReadPowerStatus();
  usr_button = ReadUserButton();

  if (loopCounter++ != 0) {                             // Ignore first reading, might be incorrect
    sprintf(buf, "%4d %3d.%02d", (loopCounter - 1) % 9999,  // Clamp to 9999,
            (int8_t)(temp / 100), (uint8_t)(temp % 100));   // Temp in decidegrees
    Serial.print(buf);
    sprintf(buf, "%3d.%03d", (int8_t)(humidity / 1000),
            (uint16_t)(humidity % 1000));  // Humidity milli-pct
    Serial.print(buf);
    sprintf(buf, "%7d.%02d", (int16_t)(pressure / 100),
            (uint8_t)(pressure % 100));  // Pressure Pascals
    Serial.print(buf);
    Serial.print(" ");
    sprintf(buf, "%4d.%02d", (int16_t)(gas / 100), (uint8_t)(gas % 100));  // Resistance milliohms
    Serial.print(buf);
    Serial.print("     ");
    Serial.print(sm_adc);
    Serial.print("     ");
    Serial.println(batt);

    float temp_f = temp / 100.0;
    float humidity_f = humidity / 1000.0;
    float pressure_f = humidity / 100.0;
    float gas_f = gas / 100.0;

    /* print to display */
    WriteToDisplay( 0,   0, "t: " + String(temp_f, 2) + " c" );
    WriteToDisplay( 0,  20, "h: " + String(humidity_f, 2) + " %" );
    WriteToDisplay( 0,  40, "p: " + String(pressure_f, 2) + " Pa" );
    WriteToDisplay( 0,  65, "g: " + String(gas_f, 1) + " mOhm");
    WriteToDisplay(-3,  90, "s: " + String(sm_adc) + " adc" );
    WriteToDisplay( 0, 110, "v: " + String(batt) + " mV" );
    WriteToDisplay( 0, 125, "ps: " + String(pwr_status) + " te: " + String(total_event_pwr_status) );
    WriteToDisplay( 0, 145, "mpps: " + String(mpp_status) + " te: " + String(total_event_mpp_status) );
    WriteToDisplay( 0, 170, "button: " + String(usr_button) + " te: " + String(total_event_user_button) );
    display.hibernate();

    delay(10000);  // Wait 10s
  }                // of ignore first reading

  /* do a full refresh */
  static uint32_t next_refresh_ms;
  if( next_refresh_ms == 0 )
  {
    next_refresh_ms = millis() + FULL_REFRESH_PERIOD_MS;
  } 
  else if ( millis() > next_refresh_ms )
  {
    FillScreen( GxEPD_WHITE );
    FillScreen( GxEPD_BLACK );
    FillScreen( GxEPD_WHITE );
    next_refresh_ms = millis() + FULL_REFRESH_PERIOD_MS;
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

void DrawHappyFace()
{
  display.setFullWindow();
  const unsigned char* bitmaps[] =
  {
    happy_face
    //logo200x200, first200x200, second200x200, fourth200x200, third200x200, fifth200x200, sixth200x200, senventh200x200, eighth200x200 // ED037TC1 test
  };

  if ((display.epd2.WIDTH == 200) && (display.epd2.HEIGHT == 200) && !display.epd2.hasColor)
  {
    bool m = display.mirror(false);
    for (uint16_t i = 0; i < sizeof(bitmaps) / sizeof(char*); i++)
    {
      display.firstPage();
      do
      {
        display.fillScreen(GxEPD_WHITE);
        display.drawInvertedBitmap(0, 0, bitmaps[i], 200, 200, GxEPD_BLACK);
      }
      while (display.nextPage());
      delay(2000);
    }
    display.mirror(m);
  }
  delay(2000);
}