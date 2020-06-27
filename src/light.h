#pragma once

#include <Arduino.h>
#include <stdint.h>

void lightSetEnable(bool state);
bool lightSetState(const char* stateName);
bool lightSetState(uint8_t stateId);
void lightInit();

uint8_t getCurrentStateId();
const char** getStateNames();
uint8_t getStateCount();