# Measurement
Power consumption and solar charging current are measured using [Nordic PPK2](https://www.nordicsemi.com/Products/Development-hardware/Power-Profiler-Kit-2)  and [Joulescope](https://joulescope.com).
The following are the summary of the measurement:
- Deep-sleep : **22uA**
- Read Sensor, Update Display, and Store Data to Flash: **31mA @ 2.5s**
- Read Sensor, Update Display, Store Data to Flash, and Publish to MQTT: **54mA @ 3.77s**
- Direct Sunlight Solar Charge: **7.5mA**
- Indirect Sunlight Solar Charge: **400uA**

## Deep-sleep
<p align="center"><img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/measurement/deep-sleep.png" alt="deep-sleep"  width="90%" height="90%"/></p>

## Solar Charge - Direct
<p align="center"><img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/measurement/simple-monitor-direct-solar-charge.png" alt="simple-monitor-direct-solar-charge"  width="90%" height="90%"/></p>

## Solar Charge - Indirect
<p align="center"><img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/measurement/test-deep-sleep-indirect-sunlight.png" alt="test-deep-sleep-indirect-sunlight"  width="90%" height="90%"/></p>

## Display Full Refresh
<p align="center"><img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/measurement/test-power-display-full.png" alt="test-power-display-full"  width="90%" height="90%"/></p>

## Read Sensor, Update Display, and Store Data to Flash
<p align="center"><img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/measurement/test-power-display-sensor-all-and-store.png" alt="test-power-display-sensor-all-and-store"  width="90%" height="90%"/></p>

##  Read Sensor, Update Display, Store Data to Flash, and Publish to MQTT
<p align="center"><img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/measurement/test-power-display-sensor-all-store-and-mqtt-publish.png" alt="test-power-display-sensor-all-store-and-mqtt-publish"  width="90%" height="90%"/></p>

## Read All Sensor
<p align="center"><img src="https://github.com/teapotlaboratories/plantpal/raw/main/docs/measurement/test-power-sensor-all.png" alt="test-power-sensor-all"  width="90%" height="90%"/></p>