/*
 * MCP23017 I/O Extension over I2C  !!! UNTESTED !!!
 */

#ifndef SER_IOEXT_HANDLER_H
#define SER_IOEXT_HANDLER_H

#include <definitions.h>

#include <MCP23017.h>
#include <list>
#include <map>
#include <string>

#include <AbstractTask.h>

#include <CarState.h>
#include <CarStatePin.h>

// known pin handler
// the handler must its bit copy to oldValue
void buttonPlusHandler();
void buttenResetHandler();
void buttonSetHandler();
void buttonMinusHandler();
void breakPedalHandler();
void fwdBwdHandler();
void mcOnOffHandler();

#endif // SER_IOEXT_HANDLER_H
