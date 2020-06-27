#include "light.h"
#include <string.h>
#include "config.h"

HardwareTimer blinkTimer(TIM4);

#define BLINK_INTERVAL  1000 // in mHz

const char* lightState[] = {
    "off",
    "red",
    "green",
    #ifdef PIN_YELLOW
    "yellow",
    "red-yellow",
    "yellow-blink"
    #endif
    "red-blink",
    "green-blink",
    "red-green-blink",
};

// sucks having to keep the two lists in sync, but eh
typedef enum {
    s_off,
    s_red,
    s_green,
    #ifdef PIN_YELLOW
    s_yellow,
    s_red_yellow,
    s_yellow_blink,
    #endif
    s_red_blink,
    s_green_blink,
    s_red_green_blink,
} state_t;

uint8_t curState = 0, prevState = s_green;
int numStates = sizeof(lightState) / sizeof(lightState[0]);
bool blinkState = 0;

uint8_t getCurrentStateId() {
    return curState;
}
const char** getStateNames() {
    return lightState;
}

uint8_t getStateCount() {
    return numStates;
}

void blinkHandler(HardwareTimer* ht) {
    switch(curState) {
        #ifdef PIN_YELLOW
        case s_yellow_blink:
            digitalWrite(PIN_YELLOW, blinkState);
            break;
        #endif
        case s_red_blink:
            digitalWrite(PIN_RED, blinkState);
            break;
        case s_green_blink:
            digitalWrite(PIN_GREEN, blinkState);
            break;
        case s_red_green_blink:
            digitalWrite(PIN_RED, !blinkState);
            digitalWrite(PIN_GREEN, blinkState);
            break;
    }
    blinkState = !blinkState;
}

void lightInit() {
    blinkTimer.attachInterrupt(blinkHandler);
    blinkTimer.setPrescaleFactor((F_CPU / 10000) - 1); // 10kHz clock
    blinkTimer.setOverflow(((10000000UL / BLINK_INTERVAL) / 2) - 1); 
    blinkTimer.resume();
}


void lightSetEnable(bool state) {
    if(state) {
        if(curState == 0) {
            lightSetState(prevState);
        }
    }
    else { 
        prevState = curState;   // save previous state
        lightSetState((uint8_t)0);       // turn lights off
    }

}

int lightStateNameToId(const char* stateName) {
    int result = -1;
    for(uint8_t i = 0; i < numStates; i++) {
        if(strcmp(lightState[i], stateName) == 0) {
            result = i;
            break;
        }
    }
    return result;
}

bool lightSetState(const char* stateName) {
    return lightSetState(lightStateNameToId(stateName));
}

inline void setYellow(bool state) {
    #ifdef PIN_YELLOW
    digitalWrite(PIN_YELLOW, state);
    #endif
}

bool lightSetState(uint8_t stateId) {
    if(!(stateId >= 0 && stateId < numStates)) {
        return false;
    }

    switch(stateId) {
        case s_off:
            digitalWrite(PIN_RED, 0);
            setYellow(0);
            digitalWrite(PIN_GREEN, 0);
            break;
        case s_red:
            digitalWrite(PIN_RED, 1);
            setYellow(0);
            digitalWrite(PIN_GREEN, 0);
            break;
        case s_green:
            digitalWrite(PIN_RED, 0);
            setYellow(0);
            digitalWrite(PIN_GREEN, 1);
            break;
        #ifdef PIN_YELLOW
        case s_yellow:
            digitalWrite(PIN_RED, 0);
            setYellow(1);
            digitalWrite(PIN_GREEN, 0);
            break;
        case s_red_yellow:
            digitalWrite(PIN_RED, 1);
            setYellow(1);
            digitalWrite(PIN_GREEN, 0);
            break;
        #endif
        default:
            digitalWrite(PIN_RED, 0);
            setYellow(0);
            digitalWrite(PIN_GREEN, 0);
            break;
    }

    curState = stateId;

    return true;
}
