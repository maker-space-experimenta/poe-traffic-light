#include <Arduino.h>
#include <SPI.h>
#include "config.h"
#include "eth.h"
#include "mqttLogic.h"
#include "light.h"

void setup() {
    DEBUG.begin(115200);
    DEBUG.println("\n\nPoE Ampel");
    pinMode(PIN_RED, OUTPUT);
    pinMode(PIN_GREEN, OUTPUT);

    lightInit();

    // init sequence
    lightSetState("red-blink");
    ethInit();
    while(!ethConnect()) {} // retry until success
    lightSetState("red");
    mqttInit();
    lightSetState("green");
    delay(500);
    lightSetState("off");
    mqttSendState();
}

void loop() {
    mqttLoop();
}