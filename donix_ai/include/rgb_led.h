#pragma once

#include <stdint.h>

const int datapin = 15;
const int clockpin = 5;
const int latchpin = 4;

void setupleds();
void send8Bits(uint8_t data);