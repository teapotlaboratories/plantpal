#include <SPI.h>
//EPD
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "Ap_29demo.h"  

// ESP32-C3 DevKit M1
// #define A14 0 //BUSY
// #define A15 1 //RES 
// #define A16 3 //DC   
// #define A17 10 //CS 

// static const uint8_t SS    = 7;
// static const uint8_t MOSI  = 6;
// static const uint8_t MISO  = 5;
// static const uint8_t SCK   = 4;

// // ESP32-C6
// #define A14 3 //BUSY
// #define A15 2 //RES 
// #define A16 11 //DC   
// #define A17 10 //CS 

// static const uint8_t SS    = 18;
// static const uint8_t MOSI  = 19;
// static const uint8_t MISO  = 20;
// static const uint8_t SCK   = 21;

// // ESP32-C6 DevKit Plantpal
// #define A14 3 //BUSY
// #define A15 2 //RES 
// #define A16 11 //DC   
// #define A17 10 //CS 

// static const uint8_t SS    = 18;
// static const uint8_t MOSI  = 21;
// static const uint8_t MISO  = 19;
// static const uint8_t SCK   = 20;

// Plantpal
#define A14 14 //BUSY
#define A15 15 //RES 
#define A16 18 //DC   
#define A17 19 //CS 

// static const uint8_t SS    = 18;
// static const uint8_t MOSI  = 21;
// static const uint8_t MISO  = 19;
// static const uint8_t SCK   = 20;

SPIClass * fspi = NULL;
void setup() {
   Serial.begin(115200);
   
   Serial.println("Start Plantpal test!");
   pinMode(A14, INPUT);  //BUSY
   pinMode(A15, OUTPUT); //RES 
   pinMode(A16, OUTPUT); //DC   
   pinMode(A17, OUTPUT); //CS   
   //SPI
  //  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0)); 
  //  SPI.begin ();  

   // VSPI
   fspi = new SPIClass(FSPI);
  //  fspi->beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0)); 
   fspi->begin(20, 16, 21, 17); //SCLK, MISO, MOSI, SS
}

//Tips//
/*
1.Flickering is normal when EPD is performing a full screen update to clear ghosting from the previous image so to ensure better clarity and legibility for the new image.
2.There will be no flicker when EPD performs a partial refresh.
3.Please make sue that EPD enters sleep mode when refresh is completed and always leave the sleep mode command. Otherwise, this may result in a reduced lifespan of EPD.
4.Please refrain from inserting EPD to the FPC socket or unplugging it when the MCU is being powered to prevent potential damage.)
5.Re-initialization is required for every full screen update.
6.When porting the program, set the BUSY pin to input mode and other pins to output mode.
// */

// void loop() {
//   delay(500);
//   Serial.print("isEPD_W21_BUSY? ");
//   Serial.println(isEPD_W21_BUSY);
// }

void loop() {
   Serial.println("Start Plantpal Full display test!");
   unsigned char i;
#if 1 //Full screen refresh, fast refresh, and partial refresh demostration.

      EPD_HW_Init(); //Full screen refresh initialization.
      EPD_WhiteScreen_White(); //Clear screen function.
      EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
      delay(2000); //Delay for 2s. 
     /************Full display(2s)*******************/
      EPD_HW_Init(); //Full screen refresh initialization.
      EPD_WhiteScreen_ALL(gImage_1); //To Display one image using full screen refresh.
      EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
      delay(2000); //Delay for 2s. 
      /************Fast refresh mode(1.5s)*******************/
      EPD_HW_Init_Fast(); //Fast refresh initialization.
      EPD_WhiteScreen_ALL_Fast(gImage_1); //To display one image using fast refresh.
      EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
      delay(2000); //Delay for 2s.
      EPD_HW_Init_Fast(); //Fast refresh initialization.
      EPD_WhiteScreen_ALL_Fast(gImage_2); //To display the second image using fast refresh.
      EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
      delay(2000); //Delay for 2s.

   Serial.println("Start Plantpal Fast refresh mode test!");
     /************Fast refresh mode(1s)*******************/
  #if 1   //To enable this feature, please change 0 to 1
      /************Onder versions do not support this feature*******************/
      EPD_HW_Init_Fast2(); //Fast refresh initialization.
      EPD_WhiteScreen_ALL_Fast2(gImage_1); //To display one image using fast refresh.
      EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
      delay(1000); //Delay for 1s.
      EPD_HW_Init_Fast2(); //EPD init Fast
      EPD_WhiteScreen_ALL_Fast2(gImage_2); //To display the second image using fast refresh.
      EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
      delay(1000); //Delay for 1s.     
  #endif
  
   Serial.println("Start Plantpal Partial refresh demostration test!");
  #if 1 //Partial refresh demostration.
  //Partial refresh demo support displaying a clock at 5 locations with 00:00.  If you need to perform partial refresh more than 5 locations, please use the feature of using partial refresh at the full screen demo.
  //After 5 partial refreshes, implement a full screen refresh to clear the ghosting caused by partial refreshes.
  //////////////////////Partial refresh time demo/////////////////////////////////////
      EPD_HW_Init(); //Electronic paper initialization. 
      EPD_SetRAMValue_BaseMap(gImage_basemap); //Please do not delete the background color function, otherwise it will cause unstable display during partial refresh.
      for(i=0;i<6;i++)
      EPD_Dis_Part_Time(64,56+32*0,Num[i],         //x-A,y-A,DATA-A
                        64,56+32*1,Num[0],         //x-B,y-B,DATA-B
                        64,56+32*2,gImage_numdot, //x-C,y-C,DATA-C
                        64,56+32*3,Num[0],        //x-D,y-D,DATA-D
                        64,56+32*4,Num[1],32,64); //x-E,y-E,DATA-E,Resolution 32*64

      EPD_DeepSleep();  //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
      delay(2000); //Delay for 2s.
      EPD_HW_Init(); //Full screen refresh initialization.
      EPD_WhiteScreen_White(); //Clear screen function.
      EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
      delay(2000); //Delay for 2s.
  #endif  
  
  #if 0 //Demo of using partial refresh to update the full screen, to enable this feature, please change 0 to 1.
  //After 5 partial refreshes, implement a full screen refresh to clear the ghosting caused by partial refreshes.
  //////////////////////Partial refresh time demo/////////////////////////////////////
      EPD_HW_Init(); //E-paper initialization 
      EPD_SetRAMValue_BaseMap(gImage_p1); //Please do not delete the background color function, otherwise it will cause an unstable display during partial refresh.
      EPD_Dis_PartAll(gImage_p1); //Image 1
      EPD_Dis_PartAll(gImage_p2); //Image 2
      EPD_Dis_PartAll(gImage_p3); //Image 3
      EPD_Dis_PartAll(gImage_p4); //Image 4
      EPD_Dis_PartAll(gImage_p5); //Image 5 
      EPD_DeepSleep();//Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
      delay(2000); //Delay for 2s. 
      EPD_HW_Init(); //Full screen refresh initialization.
      EPD_WhiteScreen_White(); //Clear screen function.
      EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
      delay(2000); //Delay for 2s.
  #endif
  
  #if 0 //Demonstration of full screen refresh with 180-degree rotation, to enable this feature, please change 0 to 1.
      /************Full display(2s)*******************/
      EPD_HW_Init_180(); //Full screen refresh initialization.
      EPD_WhiteScreen_ALL(gImage_1); //To Display one image using full screen refresh.
      EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
      delay(2000); //Delay for 2s.
  #endif        
#endif
 while(1);  // The program stops here   
}




//////////////////////////////////END//////////////////////////////////////////////////
