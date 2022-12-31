# How Tos for SER4 with ESP32 and platformio

## Run MenuConfig

writes the file `sdconfig` and sdconfig.esp32dev

`pio run -t menuconfig`

## Upload without recompile

`pio run -t nobuild -t upload --disable-auto-clean`

## Thread Safe C++

https://www.educba.com/c-thread-safe-queue/
