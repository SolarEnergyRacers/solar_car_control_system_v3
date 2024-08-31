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

## Platformio vscode

### Change `Upload` to `Upload and Monitor`

in `~/.config/Code/User/settings.json`:

change
```json
{
  "text": "$(arrow-right)",
  "tooltip": "PlatformIO: Upload",
  "commands": "platformio-ide.upload"
},
```
to
```json
{
  "text": "$(arrow-right)",
  "tooltip": "PlatformIO: Upload and Monitor",
  "commands": "platformio-ide.uploadAndMonitor"
},
```
