# HowTos for SER4 with ESP32 and platformio

## Run MenuConfig

writes the file `sdconfig` and sdconfig.esp32dev

`pio run -t menuconfig`

## Upload without recompile

`pio run -t nobuild -t upload --disable-auto-clean`

`platformio run --target upload --target monitor --environment esp32dev-linux` 

## Thread Safe C++

[c-thread-safe-queue](https://www.educba.com/c-thread-safe-queue/)

## Error: Brownout detector was triggered

[Hardware and Software Solution](https://arduino.stackexchange.com/questions/76690/esp32-brownout-detector-was-triggered-upon-wifi-begin)

## SD-Card

### Formatting

- [Notes from Adafruite](https://learn.adafruit.com/adafruit-micro-sd-breakout-board-card-tutorial/formatting-notes)
- Format: FAT32

Official SD-Card Formatters:

-  [Linux](https://www.sdcard.org/downloads/sd-memory-card-formatter-for-linux)
- [Windows/Mac](https://www.sdcard.org/downloads/formatter/)

