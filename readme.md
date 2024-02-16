# Teapotlabs Plantpal
 <p align="center"> <img src="https://media.githubusercontent.com/media/teapotlaboratories/plantpal/main/docs/images/device.jpg" alt="drawing"  width="50%" height="50%"/></p>
 
Meet Teapotlabs Plantpal -  Your pal in plant care! This adorable device uses capacitive sensing to measure soil moisture. Its low-power e-paper display keeps you updated without draining energy. Plus, with the BME688 sensor, it is able to monitor air quality and CO2 levels, ensuring your plants thrive in optimal conditions.

Teapotlabs Plantpal is part of  [Teapotlabs open-hardware project](https://github.com/teapotlaboratories). 

## Sponsorship
Huge thanks to [PCBWay](https://www.pcbway.com) to sponsored this project! All PCB and Assembly in this project is using their services.

## Acknowledgement
The project won't be possible without the amazing work from people across the globe. The following are the reference to those awesome projects!

 - Soil moisture design from [b-parasite](https://github.com/rbaron/b-parasite) 
 - Solar energy Harvesting from [TSEM](https://hackaday.io/project/159139-tiny-solar-energy-module-tsem)
 - Antenna design from [Picoclick-C3](https://makermoekoe.gitbook.io/picoclick-c3)
 
## Specification

- [ESP32-C6FH4](https://www.espressif.com/sites/default/files/documentation/esp32-c6_datasheet_en.pdf): ESP32-C6 with 4 MB in-package flash
- [AEM10941](https://e-peas.com/product/aem10941): Solar energy harvesting
- USB-C for charging and programming
- ~20uA Deep-Sleep
- 1.54 Inch E-Paper Display
- Sensor:
	- `BME688` for Environmental Sensing
	- Built-in `Soil Moisture` sensor
	- `Battery voltage` sensing
- 1A charge rate via USB
- 3.7 Volts LiPo Battery

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

## Schematics

<p align="center"> <img src="https://raw.githubusercontent.com/teapotlaboratories/plantpal/main/hardware/schematic.png" alt="schematic"/></p>

Built using **KiCAD 7.0.5**
A detailed schematic can be found [here](https://raw.githubusercontent.com/teapotlaboratories/plantpal/main/hardware/schematic.pdf)

## Boards
 <p align="center">  <img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/images/pcb_render.gif" alt="pcb_render"  width="50%" height="50%"/><br><b>PCB Render</b></p>

Built using KiCAD, the board is design to be as small as possible with all components placed on the top side of the PCB.
> ⚠️ **The following design are based on revision 3 but is similar to revision 4.**

More images can be found [here](https://github.com/teapotlaboratories/plantpal/tree/main/docs/images)

| Top Board | Bottom Board |
|--|--|
| <p align="center"> <img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/images/assembled_front.jpg" alt="assembled_front"  width="67%" height="67%"/></p> | <p align="center"> <img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/images/assembled_back.jpg" alt="assembled_back"  width="67%" height="67%"/></p> |
| <p align="center"> <img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/images/pcb_top.png" alt="pcb_front"  width="73%" height="73%"/></p> | <p align="center"> <img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/images/pcb_bottom.png" alt="pcb_bottom"  width="72%" height="72%"/></p> |

 <p align="center"> <img src="https://github.com/teapotlaboratories/plantpal/raw/main/hardware/pcb.svg" alt="pcb"  width="50%" height="50%"/><br><b>PCB Top and Bottom Layout</b></p> 
  
  
 <p align="center"> <img src="https://media.githubusercontent.com/media/teapotlaboratories/plantpal/main/docs/images/assembled_front_close_up.jpg" alt="pcb"  width="50%" height="50%"/><br><b>Assembled Front Close-up</b></p> 
 
### Case and Showcase
<p align="center">  <img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/images/case_render.gif" alt="case_render"  width="70%" height="70%"/></p>

Built using [TinkerCAD](https://www.tinkercad.com). The case are 3D printable with any generic 3D printer with/without support. The STL files are available [here](https://github.com/teapotlaboratories/plantpal/tree/main/hardware/main/case)
 <p align="center"><img src="https://media.githubusercontent.com/media/teapotlaboratories/plantpal/main/docs/images/case_open.jpg" alt="drawing"  width="50%" height="50%"/><br><b>Case Open</b></p>

`BME688` sensor is available in the back, facing 3 small vents in the back of the case
 <p align="center"><img src="https://media.githubusercontent.com/media/teapotlaboratories/plantpal/main/docs/images/assembled_back_close_up.jpg" alt="drawing"  width="70%" height="70%"/><br><b>BME688 Position</b></p>

#### Soil Moisture Sensing Showcase
Based on the amazing project in [b-parasite](https://github.com/rbaron/b-parasite), Plantpal has the built-in soil moisture sensor via capacitive sensing. Using capacitive sensing, enables the device to measure soil moisture without any traces exposed to the environment. See [rbaron's b-parasite](https://github.com/rbaron/b-parasite) project for more information on how the sensor works and more additional information to protect the soil moisture sensor!

<p align="center"><img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/images/soil_moisture_showcase.gif" alt="soil moisture sensing test"  width="50%" height="50%"/><br><b>Soil Moisture Sensing Showcase</b></p>

## Measurement
Power consumption and solar charging current are measured using [Nordic PPK2](https://www.nordicsemi.com/Products/Development-hardware/Power-Profiler-Kit-2)  and [Joulescope](https://joulescope.com).
The following are the summary of the measurement:
- Deep-sleep : **22uA**
- Read Sensor, Update Display, and Store Data to Flash: **31mA @ 2.5s**
- Read Sensor, Update Display, Store Data to Flash, and Publish to MQTT: **54mA @ 3.77s**
- Direct Sunlight Solar Charge: **7.5mA**
- Indirect Sunlight Solar Charge: **400uA**

> :warning: **Tested on Revision 4-equivalent board, but should be similar to Revision 4** 

<p align="center"><img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/measurement/deep-sleep.png" alt="deep-sleep"  width="90%" height="90%"/><br><b>Deep-sleep</b></p>

<p align="center"><img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/measurement/test-power-display-sensor-all-and-store.png" alt="power-display-sensor-all-and-store"  width="90%" height="90%"/><br><b>Read Sensor, Update Display, and Store Data to Flash</b></p>

<p align="center"><img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/measurement/test-power-display-sensor-all-store-and-mqtt-publish.png" alt="power-display-sensor-all-store-and-mqtt-publish"  width="90%" height="90%"/><br><b>Read Sensor, Update Display, Store Data to Flash, and Publish to MQTT</b></p>

<p align="center"><img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/measurement/simple-monitor-direct-solar-charge.png" alt="simple-monitor-direct-solar-charge"  width="90%" height="90%"/><br><b>Solar Charge - Direct Sunlight</b></p>

<p align="center"><img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/measurement/test-deep-sleep-indirect-sunlight.png" alt="test-deep-sleep-indirect-sunlight"  width="90%" height="90%"/><br><b>Solar Charge - Indirect Sunlight</b></p>

More measurement can be found [here](https://github.com/teapotlaboratories/plantpal/tree/main/docs/measurement)

## Bill Of Materials
Most of the components are generic and can be bought from any electornics/semi-conductor distributor. The bill of materials can be downloaded [here](https://github.com/teapotlaboratories/plantpal/blob/main/hardware/main/bill_of_materials.csv).
The EPD display used and tested is `GDEY0154D67` from **GoodDisplay**. The following display should be compatible, but the example code would need to be adjusted:
- GDEW0154T8
- GDEW0154Z17
- GDEW0154Z04
- GDEW0154C39
- GDEW0154I9F

Those displays are available from AliExpress

## Programming
Programming the device can be done easily over the USB-C. 
Out of factory, the ESP32-C6 will be blank and need to be put in `bootloader` mode. The following is a step-by-step guide to initially program the device

  1. Press and Hold `BOOT` and `RESET` button
  2. Release `RESET` button 
  3. Wait couple seconds
  4. Release `BOOT` button
  5. Plantpal should be in `bootloader` mode and you can flash it via `ESP-IDF` or `Arduino` 

A more detailed information about USB behavior on ESP32-C6 can be [here](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-guides/usb-serial-jtag-console.html)

## How to Purchase
At the moment, we do not mass produce the device. The fastest way you can get the board is to order it directly from the manufacturer [here](https://link.teapotlabs.io/plantpal-order) (does **NOT** include the display).
You need to buy the display separately from Aliexpress or other distributor.

If you'd like to buy it from us fully assembled, we might be available and please contact contact@teapotlabs.io

## Disclaimer and notes
Some disclaimer about Plantpal:
- WiFi antenna is not optimized and not certified
- RTC components have not been tested
- Deep-sleep with `esp_sleep_enable_timer_wakeup()` on Arduino SDK have weird behavior where it only sleeps for the half amount of time it instructed to do.
- The solar charge rate is 1.5mA lower than [Teapotlabs BWLR1E](https://github.com/teapotlaboratories/bwlr1e) even though they use the same components and configuration.

## Questions?
For any questions feel free to create a issue or contact contact@teapotlabs.io

## License
The product is open-source! However, some part of library used under **firmware** or **hardware**, might have it's own license.
Please reach out or create a ticket if **Teapotlabs Plantpal** violated any licenses.

![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)
