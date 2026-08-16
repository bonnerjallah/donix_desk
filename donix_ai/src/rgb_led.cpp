#include <Arduino.h>
#include "rgb_led.h"

void setupleds() {
    Serial.println("Setting up RGB LEDs...");
    
    pinMode(datapin, OUTPUT);
    pinMode(clockpin, OUTPUT);
    pinMode(latchpin, OUTPUT);
    digitalWrite(latchpin, LOW);
}

void send8Bits(uint8_t data) {
    digitalWrite(latchpin, LOW);

    for (int i = 7; i >= 0; i--) {
        digitalWrite(clockpin, LOW);

        uint8_t bitValue = (data >> i) & 1;

        digitalWrite(datapin, bitValue);

        digitalWrite(clockpin, HIGH);
    }

    digitalWrite(latchpin, HIGH);
}