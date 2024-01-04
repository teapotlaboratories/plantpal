#include <SPI.h>
#include"Ap_29demo.h"

// // ESP32-C6
// #define A14 3 //BUSY
// #define A15 2 //RES 
// #define A16 11 //DC   
// #define A17 10 //CS 

// static const uint8_t SS    = 18;
// static const uint8_t MOSI  = 19;
// static const uint8_t MISO  = 20;
// static const uint8_t SCK   = 21;

// Plantpal
// #define A14 14 //BUSY
// #define A15 15 //RES 
// #define A16 18 //DC   
// #define A17 19 //CS 

// static const uint8_t SS    = 18;
// static const uint8_t MOSI  = 21;
// static const uint8_t MISO  = 19;
// static const uint8_t SCK   = 20;

//IO settings
int BUSY_Pin = 14; 
int RES_Pin = 15; 
int DC_Pin = 18; 
int CS_Pin = 19; 

#define EPD_W21_CS_0 digitalWrite(CS_Pin,LOW)
#define EPD_W21_CS_1 digitalWrite(CS_Pin,HIGH)
#define EPD_W21_DC_0  digitalWrite(DC_Pin,LOW)
#define EPD_W21_DC_1  digitalWrite(DC_Pin,HIGH)
#define EPD_W21_RST_0 digitalWrite(RES_Pin,LOW)
#define EPD_W21_RST_1 digitalWrite(RES_Pin,HIGH)
#define isEPD_W21_BUSY digitalRead(BUSY_Pin)
////////FUNCTION//////
void SPI_Write(unsigned char value);
void EPD_W21_WriteDATA(unsigned char command);
void EPD_W21_WriteCMD(unsigned char command);
//EPD
void EPD_W21_Init(void);
void EPD_init(void);
void EPD_sleep(void);
void EPD_refresh(void);
void lcd_chkstatus(void);
void PIC_display_Clean(void);
void PIC_display(const unsigned char* picData);

SPIClass * fspi = NULL;
void setup() {
   Serial.begin(115200);
   pinMode(BUSY_Pin, INPUT); 
   pinMode(RES_Pin, OUTPUT);  
   pinMode(DC_Pin, OUTPUT);    
   pinMode(CS_Pin, OUTPUT);   
   //SPI
  //  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0)); 
  //  SPI.begin ();  

   fspi = new SPIClass(FSPI);
   fspi->beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));  
   fspi->begin(20, 16, 21, 17); //SCLK, MISO, MOSI, SS
}
//Tips//
/*When the electronic paper is refreshed in full screen, the picture flicker is a normal phenomenon, and the main function is to clear the display afterimage in the previous picture.
  When the local refresh is performed, the screen does not flash.*/
/*When you need to transplant the driver, you only need to change the corresponding IO. The BUSY pin is the input mode and the others are the output mode. */

void loop() {
  while(1)
  {

    Serial.println("Test Picture 1");
    EPD_init(); //EPD init
    PIC_display(gImage_1);//EPD_picture1
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
    delay(2000); //2s  
    
    Serial.println("Test Picture 2");
    //PICTURE2
    EPD_init(); //EPD init
    PIC_display(gImage_2);//EPD_picture1
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
     delay(2000); //2s  
    
    Serial.println("Test Clean");
    //PICTURE Clean
     EPD_init(); //EPD init
    PIC_display_Clean();
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
    while(1); 

  }


}

//////////////////////SPI///////////////////////////////////

void SPI_Write(unsigned char value)                                    
{                                                           
    fspi->transfer(value);

}

void EPD_W21_WriteCMD(unsigned char command)
{
  EPD_W21_CS_0;                   
  EPD_W21_DC_0;   // command write
  SPI_Write(command);
  EPD_W21_CS_1;
}
void EPD_W21_WriteDATA(unsigned char command)
{
  EPD_W21_CS_0;                   
  EPD_W21_DC_1;   // command write
  SPI_Write(command);
  EPD_W21_CS_1;
}


/////////////////EPD settings Functions/////////////////////
void EPD_W21_Init(void)
{
  unsigned char i;
  for(i=0;i<3;i++) //Reset, three times to prevent the secondary electricity cannot be started
  {
    EPD_W21_RST_0;    // Module reset
    delay(10);//At least 10ms delay 
    EPD_W21_RST_1;
    delay(10);//At least 10ms delay 
  }
}
void EPD_init(void)
{
    EPD_W21_Init(); //Electronic paper IC reset
    
    EPD_W21_WriteCMD(0x04);  
    lcd_chkstatus();//waiting for the electronic paper IC to release the idle signal

    EPD_W21_WriteCMD(0x00);     //panel setting
    EPD_W21_WriteDATA(0x1f);    //LUT from OTP£¬KW-BF   KWR-AF  BWROTP 0f BWOTP 1f

    EPD_W21_WriteCMD(0x61);     //resolution setting
    EPD_W21_WriteDATA (0x98);   //152        
    EPD_W21_WriteDATA (0x00);   //152
    EPD_W21_WriteDATA (0x98);
  
    EPD_W21_WriteCMD(0X50);     //VCOM AND DATA INTERVAL SETTING      
    EPD_W21_WriteDATA(0x97);    //WBmode:VBDF 17|D7 VBDW 97 VBDB 57   WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7
}
void EPD_refresh(void)
{
    EPD_W21_WriteCMD(0x12);     //DISPLAY REFRESH   
    delay(10);          //!!!The delay here is necessary, 200uS at least!!!     
    lcd_chkstatus();
} 
void EPD_sleep(void)
{
    EPD_W21_WriteCMD(0X02);   //power off
    lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal
    //Part2 Increase the time delay
    delay(1000);      //Power off time delay, this is  necessary!!!     
    EPD_W21_WriteCMD(0X07);   //deep sleep
    EPD_W21_WriteDATA(0xA5);
}

void PIC_display(const unsigned char* picData)
{
    unsigned int i;
    EPD_W21_WriteCMD(0x10);        //Transfer old data
    for(i=0;i<2888;i++)      
    EPD_W21_WriteDATA(0xff); 
  
    EPD_W21_WriteCMD(0x13);        //Transfer new data
    for(i=0;i<2888;i++)      
     EPD_W21_WriteDATA(pgm_read_byte(&picData[i]));

       //Refresh
    EPD_W21_WriteCMD(0x12);   //DISPLAY REFRESH   
    delay(10);    //!!!The delay here is necessary, 200uS at least!!!     
    lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal

}

void PIC_display_Clean(void)
{
    unsigned int i;
    EPD_W21_WriteCMD(0x10);        //Transfer old data
    for(i=0;i<2888;i++)       
  {
    EPD_W21_WriteDATA(0xff);
  }
  
    EPD_W21_WriteCMD(0x13);        //Transfer new data
    for(i=0;i<2888;i++)       
  {
    EPD_W21_WriteDATA(0xff);
  }
     //Refresh
    EPD_W21_WriteCMD(0x12);   //DISPLAY REFRESH   
    delay(10);    //!!!The delay here is necessary, 200uS at least!!!     
    lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal

}
void lcd_chkstatus(void)
{
  while(1)
  {   //=0 BUSY
     if(isEPD_W21_BUSY==1) break;
  }                  
}
