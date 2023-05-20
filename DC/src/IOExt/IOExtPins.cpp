//
// MCP23017 I/O Extension over I2C
//
#include <definitions.h>

#include <CarControl.h>
#include <Helper.h>
#include <IOExt.h>
#include <IOExtHandler.h>

array<CarStatePin,IOExtPINCOUNT> CarState::pins = set_pins_defs();

array<CarStatePin,IOExtPINCOUNT> CarState::set_pins_defs() {
    array<CarStatePin,IOExtPINCOUNT> data;
    // v = value
    // ov = oldValue
    // ts = timestamp
    // cmode = continuous mode
    // dbt = debounce time (==0 for toggles!)
    //                    port (device|pin), mask, I/O mode,     v,ov,inited, ts, name,               handler,            cmode, dbt
    // IOExtDev0-PortA (main board)
    data[IOExt_PinIds::BTN_PLUS]    = {0x00, 0x01, INPUT_PULLUP, 0, 0, false, 0l, PinDI_Button_Plus,  buttonPlusHandler,  true,  200};
    data[IOExt_PinIds::BTN_RESET]   = {0x01, 0x02, INPUT_PULLUP, 0, 0, false, 0l, PinDI_Button_Reset, buttenResetHandler, true,  200};
    data[IOExt_PinIds::BTN_MINUS]   = {0x02, 0x04, INPUT_PULLUP, 0, 0, false, 0l, PinDI_Button_Minus, buttonMinusHandler, true,  200};
    data[IOExt_PinIds::BTN_SET]     = {0x03, 0x08, INPUT_PULLUP, 0, 0, false, 0l, PinDI_Button_Set,   buttonSetHandler,   false, 200};
    //
    data[IOExt_PinIds::PEDAL_BRAKE] = {0x04, 0x10, INPUT_PULLUP, 0, 0, false, 0l, PinDI_Break,        breakPedalHandler,  false, 0};
    data[IOExt_PinIds::LIGHT_BRAKE] = {0x05, 0x20, OUTPUT,       1, 1, false, 0l, PinDO_BreakLight,   NULL,               false, 200};
    data[IOExt_PinIds::SW_FWD_BWD]  = {0x06, 0x40, INPUT,        0, 0, false, 0l, PinDI_FWD_BWD,      fwdBwdHandler,      false, 0};  // z-diode -> no pullup
    data[IOExt_PinIds::SW_MC_ON]    = {0x07, 0x80, INPUT,        0, 0, false, 0l, PinDI_MCONOFF,      mcOnOffHandler,     false, 0};  // z-diode -> no pullup
    // IOExtDev0-PortB (main board)
    data[IOExt_PinIds::PIN_GPB0]    = {0x08, 0x01, INPUT_PULLUP, 0, 0, false, 0l, PinGPB0,            NULL,               false, 200};
    data[IOExt_PinIds::PIN_GPB1]    = {0x09, 0x02, INPUT_PULLUP, 0, 0, false, 0l, PinGPB1,            NULL,               false, 200};
    data[IOExt_PinIds::PIN_GPB2]    = {0x0a, 0x04, INPUT_PULLUP, 0, 0, false, 0l, PinGPB2,            NULL,               false, 200};
    data[IOExt_PinIds::PIN_GPB3]    = {0x0b, 0x08, INPUT_PULLUP, 0, 0, false, 0l, PinGPB3,            NULL,               false, 200};
    //
    data[IOExt_PinIds::PIN_GPB4]    = {0x0c, 0x10, INPUT_PULLUP, 0, 0, false, 0l, PinGPB4,            NULL,               false, 200};
    data[IOExt_PinIds::PIN_GPB5]    = {0x0d, 0x20, INPUT_PULLUP, 0, 0, false, 0l, PinGPB5,            NULL,               false, 200};
    data[IOExt_PinIds::PIN_GPB6]    = {0x0e, 0x40, INPUT_PULLUP, 0, 0, false, 0l, PinGPB6,            NULL,               false, 200};
    data[IOExt_PinIds::PIN_GPB7]    = {0x0f, 0x80, INPUT_PULLUP, 0, 0, false, 0l, PinGPB7,            NULL,               false, 200};
    return data;
}

/*

CarStatePin CarState::pins[IOExtPINCOUNT] = {
    // port (device|pin), mask, I/O mode, value, oldValue, inited, timestamp, name, 
    // handler, continouse mode, debaunce time (==0 for toggles!)
    //
    // IOExtDev0-PortA (main board)
    {0x00, 0x01, INPUT_PULLUP, 0, 0, false, 0l, PinDI_Button_Plus, buttonPlusHandler, true, 200},
    {0x01, 0x02, INPUT_PULLUP, 0, 0, false, 0l, PinDI_Button_Reset, buttenResetHandler, false, 200},
    {0x02, 0x04, INPUT_PULLUP, 0, 0, false, 0l, PinDI_Button_Minus, buttonMinusHandler, true, 200},
    {0x03, 0x08, INPUT_PULLUP, 0, 0, false, 0l, PinDI_Button_Set, buttonSetHandler, false, 200},
    //
    {0x04, 0x10, INPUT_PULLUP, 0, 0, false, 0l, PinDI_Break, breakPedalHandler, false, 0},
    {0x05, 0x20, OUTPUT, 1, 1, false, 0l, PinDO_BreakLight, NULL, false, 200},
    {0x06, 0x40, INPUT, 0, 0, false, 0l, PinDI_FWD_BWD, fwdBwdHandler, false, 0},  // don't pull z-diode!
    {0x07, 0x80, INPUT, 0, 0, false, 0l, PinDI_MCONOFF, mcOnOffHandler, false, 0}, // don't pull z-diode!
    // IOExtDev0-PortB (main board)
    {0x08, 0x01, INPUT_PULLUP, 0, 0, false, 0l, PinGPB0, NULL, false, 200},
    {0x09, 0x02, INPUT_PULLUP, 0, 0, false, 0l, PinGPB1, NULL, false, 200},
    {0x0a, 0x04, INPUT_PULLUP, 0, 0, false, 0l, PinGPB2, NULL, false, 200},
    {0x0b, 0x08, INPUT_PULLUP, 0, 0, false, 0l, PinGPB3, NULL, false, 200},
    //
    {0x0c, 0x10, INPUT_PULLUP, 0, 0, false, 0l, PinGPB4, NULL, false, 200},
    {0x0d, 0x20, INPUT_PULLUP, 0, 0, false, 0l, PinGPB5, NULL, false, 200},
    {0x0e, 0x40, INPUT_PULLUP, 0, 0, false, 0l, PinGPB6, NULL, false, 200},
    {0x0f, 0x80, INPUT_PULLUP, 0, 0, false, 0l, PinGPB7, NULL, false, 200}};

*/