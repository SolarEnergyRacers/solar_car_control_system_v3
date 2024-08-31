# Fixes to loaded Library Packages 

[TOC]

platform: espressif32@5.2.0

## General / Tools

### Upload without rebuild

`pio run -t nobuild -t upload`

### Rebuild all every time Error

#### Error

All sources and libraries are built every time the compile or compile, upload, monitor is executed.

#### Files affeced

~~.platformio/penv/lib/python3.8/site-packages/platformio/builder/main.py:~~

```python
#env.SConsignFile(
#    os.path.join(
#        "$BUILD_DIR", ".sconsign%d%d" % (sys.version_info[0], sys.version_info[1])
#    )
#)
#
env.SConsignFile(
    os.path.join(
        "$BUILD_DIR", ".sconsign.dblite")
    )
)
```

#### Solution

remove `-D CURRENT_TIME=$UNIX_TIME` from `build_flags`

## AC + DC

### Conflict concerning `#define B1 1`

#### Error Message

```bash
/home/ksc/.platformio/packages/framework-arduinoespressif32/cores/esp32/binary.h:31:12: error: expected '>' before numeric constant
 #define B1 1
            ^
.pio/libdeps/esp32dev-linux/fmt/include/fmt/core.h:1118:16: note: in expansion of macro 'B1'
 template <bool B1, bool B2, bool... Tail> constexpr auto count() -> size_t {
                ^~
```

#### Files affeced

- File1: `.platformio/packages/framework-arduinoespressif32/cores/esp32/binary.h`
- File2:
  -  Linux: `.pio/libdeps/esp32dev-linux/fmt/include/fmt/core.h`
  - Windows:  `.pio/libdeps/esp32dev-windows/fmt/include/fmt/core.h`

#### FIX

To avoid problem in `binary.h` change the following file:

- Change for Linux: `.pio/libdeps/esp32dev-linux/fmt/include/fmt/core.h` at line 1120ff
- Change for Windows: `.pio/libdeps/esp32dev-windows/fmt/include/fmt/core.h` at line 1120ff

```c++
// marker line:
template <bool B = false> constexpr auto count() -> size_t { return B ? 1 : 0; }
// CHANGE_KSC: Conflict with #define B1 1 in .platformio/packages/framework-arduinoespressif32/cores/esp32/binary.h
template <bool BOOL1, bool BOOL2, bool... Tail> constexpr auto count() -> size_t {
  return (BOOL1 ? 1 : 0) + count<BOOL2, Tail...>();
}
```

### ~~esp32-hal-gpio.c~~

#### ~~Error Message~~

???

#### ~~Files affeced~~

```bash
/home/ksc/.platformio/packages/framework-arduinoespressif32/cores/esp32/esp32-hal-gpio.c: In function '__pinMode':
/home/ksc/.platformio/packages/framework-arduinoespressif32/cores/esp32/esp32-hal-gpio.c:102:24: error: 'rtc_gpio_desc' undeclared (first use in this function); did you mean 'rtc_io_desc'?
```

[issues57](https://github.com/maximkulkin/esp32-homekit-camera/issues/57)

#### FIX 

- There is a setting on latest ESP-IDF you need to enable for this project. I’m afk now, but you should be able to find it somewhere in component configs -> device configurations -> rtc something. Just enable it and it will compile.
- It is in `Component config > driver configurations > RTCI0 configuration`: \[*] Support array `rtc_gpio_desc` for ESP32

### rtc.h

#### Error Message

???

#### Files affected

#### FIX

Comment `#define MHZ (1000000)` to avoid conflict in ???

### .platformio/packages/framework-arduinoespressif32/libraries/ESPmDNS/src/ESPmDNS.h

```c++
//KSC void enableWorkstation(wifi_interface_t interface=ESP_IF_WIFI_STA);
  void enableWorkstation(wifi_interface_t interface);
  void disableWorkstation();
```

### ESPmDNS

#### Error Message

???

#### Files affected

- `.platformio/packages/framework-arduinoespressif32/libraries/ESPmDNS/src/ESPmDNS.h`

- `.platformio/packages/framework-arduinoespressif32/libraries/ESPmDNS/src/ESPmDNS.cpp` 

#### FIX

Change in `.platformio/packages/framework-arduinoespressif32/libraries/ESPmDNS/src/ESPmDNS.h`:

```c++
  //KSC void enableWorkstation(wifi_interface_t interface=ESP_IF_WIFI_STA);
  void enableWorkstation(wifi_interface_t interface);
  void disableWorkstation();
```

Change in `.platformio/packages/framework-arduinoespressif32/libraries/ESPmDNS/src/ESPmDNS.cpp`:

```c++
IPAddress MDNSResponder::queryHost(char *host, uint32_t timeout){
    //KSC struct ip4_addr addr;
    struct esp_ip4_addr addr;
    addr.addr = 0;

    esp_err_t err = mdns_query_a(host, timeout,  &addr);
    if(err){
        if(err == ESP_ERR_NOT_FOUND){
            log_w("Host was not found!");
            return IPAddress();
        }
        log_e("Query Failed");
        return IPAddress();
    }
    return IPAddress(addr.addr);
}
```

### ssl_client.cpp

#### Error Message

???

#### Files affected

- `.platformio/packages/framework-arduinoespressif32/libraries/WiFiClientSecure/src/ssl_client.cpp`

#### FIX

- [Fix with PR](https://github.com/gravitech-engineer/AIS_IoT_4G/pull/8)

Change in `.platformio/packages/framework-arduinoespressif32/libraries/WiFiClientSecure/src/ssl_client.cpp`:

```c++
//KSC #ifndef MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED
#if !defined(MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED) && !defined(MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED)
#  error "Please configure IDF framework to include mbedTLS -> Enable pre-shared-key ciphersuites and activate at least one cipher"
#endif
```

### Several things made with pio menuconfig

#### Error Message

Please call `idf.py menuconfig` then go to Component config -> mbedTLS -> TLS Key Exchange Methods -> Enable pre-shared-key ciphersuites and then check `Enable PSK based cyphersuite modes`. Save and Quit."

#### Files affected

```make
/home/ksc/.platformio/packages/framework-arduinoespressif32/libraries/WiFiClientSecure/src/ssl_client.cpp:24:4: warning: #warning "Please call `idf.py menuconfig` then go to Component config -> mbedTLS -> TLS Key Exchange Methods -> Enable pre-shared-key ciphersuites and then check `Enable PSK based cyphersuite modes`. Save and Quit." [-Wcpp]
 #  warning "Please call `idf.py menuconfig` then go to Component config -> mbedTLS -> TLS Key Exchange Methods -> Enable pre-shared-key ciphersuites and then check `Enable PSK based cyphersuite modes`. Save and Quit."
    ^~~~~~~
```

## AC only

## DC only
