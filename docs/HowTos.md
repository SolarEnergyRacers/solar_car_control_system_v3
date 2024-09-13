# HowTos for SER5 with ESP32 and platformio

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

behind

```json
{
  "text": "$(arrow-right)",
  "tooltip": "PlatformIO: Upload",
  "commands": "platformio-ide.upload"
},
```

add

```json
{
  "text": "$(arrow-right)$(plug)",
  "tooltip": "PlatformIO: Upload and Monitor",
  "commands": "platformio-ide.uploadAndMonitor"
},
```

**complete `platformio-ide` part in** `~/.config/Code/User/settings.json`:

```json
    "platformio-ide.forceUploadAndMonitor": true,
    "platformio-ide.disablePIOHomeStartup": true,
    "platformio-ide.customPATH": "",
    "platformio-ide.toolbar": [
        {
            "text": "$(home)",
            "tooltip": "PlatformIO: Home",
            "commands": "platformio-ide.showHome"
        },
        {
            "text": "$(check)",
            "tooltip": "PlatformIO: Build",
            "commands": "platformio-ide.build"
        },
        {
            "text": "$(arrow-right)",
            "tooltip": "PlatformIO: Upload",
            "commands": "platformio-ide.upload"
        },
        {
            "text": "$(arrow-right)$(plug)",
            "tooltip": "PlatformIO: Upload and Monitor",
            "commands": "platformio-ide.uploadAndMonitor"
        },
        {
            "text": "$(trashcan)",
            "tooltip": "PlatformIO: Clean",
            "commands": "platformio-ide.clean"
        },
        {
            "text": "$(beaker)",
            "tooltip": "PlatformIO: Test",
            "commands": "platformio-ide.test"
        },
        {
            "text": "$(plug)",
            "tooltip": "PlatformIO: Serial Monitor",
            "commands": "platformio-ide.serialMonitor"
        },
        {
            "text": "$(terminal)",
            "tooltip": "PlatformIO: New Terminal",
            "commands": "platformio-ide.newTerminal"
        }
    ],
    "platformio-ide.reopenSerialMonitorDelay": 1000,

```

