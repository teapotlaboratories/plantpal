


# Teapotlabs Plantpal
 <p align="center"> <img src="https://media.githubusercontent.com/media/teapotlaboratories/plantpal/main/docs/images/device.jpg" alt="drawing"  width="50%" height="50%"/></p>
 
Meet Teapotlabs Plantpal - the compact wonder for your garden! This adorable device uses capacitive sensing to measure soil moisture. Its low-power e-paper display keeps you updated without draining energy. Plus, with the BME688 sensor, it is able to monitor air quality and CO2 levels, ensuring your plants thrive in optimal conditions. Precision gardening made cute!

Teapotlabs Plantpal is part of  [Teapotlabs open-hardware project](https://github.com/teapotlaboratories). 

## Sponsorship
Huge thanks to [PCBWay](https://www.pcbway.com) to sponsored this project! 
All PCB and Assembly in this project is using their services.

## Acknowledgement
The project won't be possible without the amazing work from people across the globe. The following are the reference to those awesome projects!

 - Soil moisture design from [b-parasite](https://github.com/rbaron/b-parasite) 
 - Solar energy Harvesting from [TSEM](https://hackaday.io/project/159139-tiny-solar-energy-module-tsem)
 - Antenna design from [Picoclick-C3](https://makermoekoe.gitbook.io/picoclick-c3)
  
## Revision
- **Revision 1**: Initial public release
- **Revision 2**: Remove via in pad
- **Revision 3**: 
	- Fix incorrect BOM component for D1
	- Fix D6 Polarity
	- Remove Power Gate
	- Change **Soil Sensor** `SENSE_OUT` pin
	- Add EPD power switch
	- Change to 4-layer board
	- Move BME688 to bottom layer
	- Improve antenna layout
- **Revision 4**: Re-wire EPD remaining power to +3V3D

## Specification

- [ESP32-C6FH4](https://www.espressif.com/sites/default/files/documentation/esp32-c6_datasheet_en.pdf): ESP32-C6 with 4 MB in-package flash
- [AEM10941](https://e-peas.com/product/aem10941): Solar energy harvesting
- USB-C for charging and programming
- ~16uA Deep-Sleep
- 1.54 Inch E-Paper Display
- Sensor:
	- `BME688` for Environmental Sensing
	- Built-in `Soil Moisture` sensor
	- `Battery voltage` sensing
- 1A charge rate via USB
- 3.7 Volts LiPo Battery

## Schematics

<p align="center"> <img src="https://raw.githubusercontent.com/teapotlaboratories/plantpal/main/hardware/schematic.png" alt="schematic"/></p>

Built using **KiCAD 7.0.5**
A detailed schematic can be found [here](https://raw.githubusercontent.com/teapotlaboratories/plantpal/main/hardware/schematic.pdf)

## Boards
 <p align="center">  <img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/images/pcb_render.gif" alt="pcb_render"  width="50%" height="50%"/><br><b>PCB Render</b></p>

Built using KiCAD, the board is design to be as small as possible with all components placed on the top side of the PCB.

The following design are based on the latest revision.
| Top Board | Bottom Board |
|--|--|
| <p align="center"> <img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/images/assembled_front.jpg" alt="assembled_front"  width="67%" height="67%"/></p> | <p align="center"> <img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/images/assembled_back.jpg" alt="assembled_back"  width="67%" height="67%"/></p> |
| <p align="center"> <img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/images/pcb_top.png" alt="pcb_front"  width="73%" height="73%"/></p> | <p align="center"> <img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/images/pcb_bottom.png" alt="pcb_bottom"  width="72%" height="72%"/></p> |

 <p align="center"> <img src="https://github.com/teapotlaboratories/plantpal/raw/main/hardware/pcb.svg" alt="pcb"  width="50%" height="50%"/><br><b>PCB Top and Bottom Layout</b></p> 
  
  
 <p align="center"> <img src="https://media.githubusercontent.com/media/teapotlaboratories/plantpal/main/docs/images/assembled_front_close_up.jpg" alt="pcb"  width="50%" height="50%"/><br><b>Assembled Front Close-up</b></p> 
 
### Case and Showcase
<p align="center">  <img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/images/case_render.gif" alt="case_render"  width="70%" height="70%"/></p>

Built using [TinkerCAD](https://www.tinkercad.com). The case are 3D printable with any generic 3D printer with/without support. The STL files are available [here](https://github.com/teapotlaboratories/bwlr1e/tree/main/hardware/main/case)
 <p align="center"><img src="https://media.githubusercontent.com/media/teapotlaboratories/plantpal/main/docs/images/case_open.jpg" alt="drawing"  width="50%" height="50%"/><br><b>Case Open</b></p>

`BME688` sensor is available in the back, facing 3 small vents in the back of the case
 <p align="center"><img src="https://media.githubusercontent.com/media/teapotlaboratories/plantpal/main/docs/images/assembled_back_close_up.jpg" alt="drawing"  width="50%" height="50%"/><br><b>BME688 Position</b></p>

#### Soil Moisture Sensing Showcase
Based on the amazing project in [b-parasite](https://github.com/rbaron/b-parasite), Plantpal has the built-in soil moisture sensor via capacitive sensing. Using capacitive sensing, enables the device to measure soil moisture without any traces exposed to the environment. See [rbaron's b-parasite](https://github.com/rbaron/b-parasite) project for more information on how the sensor works and more additional information to protect the soil moisture sensor!

<p align="center"><img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/images/soil_moisture_showcase.gif" alt="soil moisture sensing test"  width="50%" height="50%"/><br><b>Soil Moisture Sensing Showcase</b></p>

### Measurement
Power consumption and solar charging current are measured using [Nordic PPK2](https://www.nordicsemi.com/Products/Development-hardware/Power-Profiler-Kit-2)  and [CurrentRanger](https://lowpowerlab.com/shop/product/152).
The following are the summary of the measurement:
- Transmit 14dBm:  305ms @ 20mA
- Deep-Sleep : 3.22 uA
- Direct Sunlight Solar Charge: 9mA
- Indirect Sunlight Solar Charge: 300uA

> :warning: **Tested on Revision 3, but should be similar to Revision 5** 

<p align="center"><img src="https://github.com/teapotlaboratories/bwlr1e/raw/main/docs/measurement/deep_sleep.png" alt="deep_sleep"  width="90%" height="90%"/><br><b>Deep-Sleep</b></p>

<p align="center"><img src="https://github.com/teapotlaboratories/bwlr1e/raw/main/docs/measurement/bme680_measure_and_lora_transmit.png" alt="bme688_measure_and_lora_transmit"  width="90%" height="90%"/><br><b>BME688 Measure and LoRa Transmit</b></p>

| Solar Charge - Direct Sunlight | Solar Charge - Indirect Sunlight |
|--|--|
| <p align="center"> <img src="https://github.com/teapotlaboratories/bwlr1e/raw/main/docs/measurement/charge_direct_sunlight.jpg" alt="assembled_front"  width="80%" height="80%"/></p> | <p align="center"> <img src="https://github.com/teapotlaboratories/bwlr1e/raw/main/docs/measurement/charge_indirect_sunlight.jpg" alt="assembled_back"  width="70%" height="70%"/></p> |

More measurement can be found [here](https://github.com/teapotlaboratories/bwlr1e/tree/main/docs/measurement)

### Bill Of Materials
Most of the components are generic and can be bought from any electornics/semi-conductor distributor. The bill of materials can be downloaded [here](https://github.com/teapotlaboratories/bwlr1e/blob/main/hardware/main/bill_of_materials.csv)

## Programming

> :warning: **Board can only be powered when LiPo battery is connected** 

Programming the device can be done easily over the USB-C. over the **UART2** or **SWD**, available on the right side of the board or uses the [BWLR1E Programmer](https://github.com/teapotlaboratories/bwlr1e/blob/main/readme_programmer.md).
Out of the factory, the RAK3172 chip ships with an **AT firmware** that can be tested by connecting a USB-to-UART bridge to the **UART2** port.

The following are some very good tutorial to start developing with the device:

- [Communicating with the AT firmware](https://docs.rakwireless.com/Product-Categories/WisDuo/RAK3172-Module/Quickstart/#rak3172-as-a-lora-lorawan-modem-via-at-command)
 - [Programming with Arduino](https://docs.rakwireless.com/Product-Categories/WisDuo/RAK3172-Module/Quickstart/#rak3172-as-a-stand-alone-device-using-rui3)
 - [Programming with STM32Cube](https://docs.rakwireless.com/Product-Categories/WisDuo/RAK3172-Module/Low-Level-Development/#rak3172-on-stm32cubeide-with-stm32wl-sdk-v1-0-0)
 - [Programming with MbedOS](https://github.com/hallard/LoRa-E5-Tiny/blob/main/README.md#compile-and-flash-firmware)


### Available SDK
Through **UART2**, user can program the device using the following SDK:
- [Arduino](https://github.com/espressif/arduino-esp32) ( at the moment only available on version `3.0.0.0-alpha3` )
- [ESP-IDF](https://github.com/espressif/esp-idf)

Out of factory the device need to be put in bootloader mode, to To program the device, user need to put the device in Serial Bootloader mode by,
1. Press and Hold `BOOT` and `RESET` button
2. Release `RESET` button 
3. Wait few seconds ( try 2 seconds )
4. Release `BOOT` button
5. Device should be in Serial Bootloader mode.

You can also use [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html) , instead of the SDK  to upload a firmware

## License
The product is open-source! However, some part of library used under **src**, might have it's own license.
Please reach out or create a ticket to report any license violation.

![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)