# Fixes to loaded Library Packages for platform = espressif32@5.2.0

## esp32-hal-gpio.c

```bash
/home/ksc/.platformio/packages/framework-arduinoespressif32/cores/esp32/esp32-hal-gpio.c: In function '__pinMode':
/home/ksc/.platformio/packages/framework-arduinoespressif32/cores/esp32/esp32-hal-gpio.c:102:24: error: 'rtc_gpio_desc' undeclared (first use in this function); did you mean 'rtc_io_desc'?
```

[](https://github.com/maximkulkin/esp32-homekit-camera/issues/57)

FIX:  

- There is a setting on latest ESP-IDF you need to enable for this project. I’m afk now, but you should be able to find it somewhere in component configs -> device configurations -> rtc something. Just enable it and it will compile.
- It is in `Component config > driver configurations > RTCI0 configuration`: \[*] Support array `rtc_gpio_desc` for ESP32

## .pio/libdeps/esp32dev/fmt/include/fmt/core.h

Conflict with #define B1 1 in .platformio/packages/framework-arduinoespressif32/cores/esp32/binary.h

Change in core.h at line 1120ff

FIX:

```c++
template <bool B = false> constexpr auto count() -> size_t { return B ? 1 : 0; }
// CHANGE_KSC: Conflict with #define B1 1 in .platformio/packages/framework-arduinoespressif32/cores/esp32/binary.h
template <bool BOOL1, bool BOOL2, bool... Tail> constexpr auto count() -> size_t {
  return (BOOL1 ? 1 : 0) + count<BOOL2, Tail...>();
}
```

## rtc.h

FIX:

Comment `#define MHZ (1000000)` to avoid conflict in ???

## .platformio/packages/framework-arduinoespressif32/libraries/ESPmDNS/src/ESPmDNS.h
 
```c++
//KSC void enableWorkstation(wifi_interface_t interface=ESP_IF_WIFI_STA);
  void enableWorkstation(wifi_interface_t interface);
  void disableWorkstation();
```

## .platformio/packages/framework-arduinoespressif32/libraries/ESPmDNS/src/ESPmDNS.cpp

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

## ssl_client.cpp

- [Fix with PR](https://github.com/gravitech-engineer/AIS_IoT_4G/pull/8)

`.platformio/packages/framework-arduinoespressif32/libraries/WiFiClientSecure/src/ssl_client.cpp`

```c++
//KSC #ifndef MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED
#if !defined(MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED) && !defined(MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED)
#  error "Please configure IDF framework to include mbedTLS -> Enable pre-shared-key ciphersuites and activate at least one cipher"
#endif
```

## Several made with pio menuconfig

```make
/home/ksc/.platformio/packages/framework-arduinoespressif32/libraries/WiFiClientSecure/src/ssl_client.cpp:24:4: warning: #warning "Please call `idf.py menuconfig` then go to Component config -> mbedTLS -> TLS Key Exchange Methods -> Enable pre-shared-key ciphersuites and then check `Enable PSK based cyphersuite modes`. Save and Quit." [-Wcpp]
 #  warning "Please call `idf.py menuconfig` then go to Component config -> mbedTLS -> TLS Key Exchange Methods -> Enable pre-shared-key ciphersuites and then check `Enable PSK based cyphersuite modes`. Save and Quit."
    ^~~~~~~
```

## Upload withour rebuild

`pio run -t nobuild -t upload`

## Rebuild all everytime

.platformio/penv/lib/python3.8/site-packages/platformio/builder/main.py:

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
