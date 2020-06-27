#include "config_secrets.h"


#define PIN_RED     PB3
#define PIN_GREEN   PB4
// #define PIN_YELLOW  PB5

#define DEBUG       Serial1

#define DO_DHCP
#ifndef DO_DHCP
    #define STATIC_IP 192,168,178,100
#endif


#define SPI2_MOSI   PB15
#define SPI2_MISO   PB14
#define SPI2_SCK    PB13
#define SPI2_CS     PB12