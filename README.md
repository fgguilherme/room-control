# room-control

Esp32 based device to control 8relay board throught MQTT protocol.

## How to run

* Download and install ESPRESSIF Toolchain for ESP32 from https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/

* after install and export, update relay ports and MQTT broker:

    * cd room_control

    * idf.py fullclean
    
    * idf.py menuconfig

    * idf.py build

@TODO

- [ ] add topics on menuconfig
- [ ] add BT capabilities (proximity and commands)
- [ ] integrate more sensors (Temperature BME180, PIR, Light sensor (day/night))
- [ ] integrate NTC to double check day/night and provide timestamps


## Used Libraries

Libraries/Components included at components folder:

* https://github.com/UncleRus/esp-idf-lib
* https://github.com/tonyp7/esp32-wifi-manager