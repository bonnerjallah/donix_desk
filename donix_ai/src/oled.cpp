#include <Arduino.h>
#include "oled.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <FluxGarage_RoboEyes.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SH1106G display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    OLED_RESET
);

RoboEyes<Adafruit_SH1106G> roboEyes(display);

void setupOLED() {

    Serial.println("Setting up OLED...");

    Wire.begin(32, 33);

    delay(250);

    if (!display.begin(0x3C, true)) {
        Serial.println("OLED initialization failed!");
        return;
    }

    Serial.println("OLED initialized.");

    // -------------------------
    // STARTUP SCREEN
    // -------------------------

    display.clearDisplay();

    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(10, 25);
    display.println("DONIX-AI");

    display.display();

    // Keep startup screen visible
    delay(2000);

    // -------------------------
    // START ROBO EYES
    // -------------------------

    roboEyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 100);

    roboEyes.setAutoblinker(ON, 3, 2);
    roboEyes.setIdleMode(ON, 2, 2);
}

void roboEyesUpdate() {
    roboEyes.update();
}