#ifndef _DISPLAY_EPD_W21_SPI_
#define _DISPLAY_EPD_W21_SPI_
#include "Arduino.h"

#define A14 14 //BUSY
#define A15 15 //RES 
#define A16 18 //DC   
#define A17 19 //CS 

//IO settings
//SCK--GPIO23(SCLK)
//SDIN---GPIO18(MOSI)
#define isEPD_W21_BUSY digitalRead(A14)  //BUSY
#define EPD_W21_RST_0 digitalWrite(A15,LOW)  //RES
#define EPD_W21_RST_1 digitalWrite(A15,HIGH)
#define EPD_W21_DC_0  digitalWrite(A16,LOW) //DC
#define EPD_W21_DC_1  digitalWrite(A16,HIGH)
#define EPD_W21_CS_0 digitalWrite(A17,LOW) //CS
#define EPD_W21_CS_1 digitalWrite(A17,HIGH)


void SPI_Write(unsigned char value);
void EPD_W21_WriteDATA(unsigned char datas);
void EPD_W21_WriteCMD(unsigned char command);


#endif 
