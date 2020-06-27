#pragma once
#include <SPI.h>
#include <Ethernet.h>

#include "config.h"

extern uint8_t macAddr[6];
extern char macStr[13];
extern EthernetClient net;

void ethInit();
bool ethConnect();