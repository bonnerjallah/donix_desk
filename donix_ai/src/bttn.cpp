#include <Arduino.h>
#include "bttn.h"

const int bttnPin = 13;

namespace Button {

    constexpr unsigned long DEBOUNCE_DELAY = 50;
    constexpr unsigned long DOUBLE_CLICK_TIME = 400;
    constexpr unsigned long LONG_PRESS_TIME = 1000;

    bool buttonState = HIGH;
    bool lastButtonState = HIGH;

    unsigned long lastDebounceTime = 0;
    unsigned long lastClickTime = 0;
    unsigned long pressStartTime = 0;

    bool waitingForSecondClick = false;
    bool secondClickInProgress = false;

    bool longPressDetected = false;
}


// =========================
// Setup
// =========================

void setupButton() {

    pinMode(bttnPin, INPUT_PULLUP);

    Button::buttonState = digitalRead(bttnPin);
    Button::lastButtonState = Button::buttonState;

    Button::lastDebounceTime = millis();
}


// =========================
// Update Button
// =========================

ButtonEvent updateButton() {

    bool currentState = digitalRead(bttnPin);
    unsigned long currentTime = millis();


    // =========================
    // Raw state changed
    // =========================

    if (currentState != Button::lastButtonState) {

        Button::lastDebounceTime = currentTime;
    }


    // =========================
    // Debounce
    // =========================

    if ((currentTime - Button::lastDebounceTime) >= Button::DEBOUNCE_DELAY) {

        // =========================
        // Stable state changed
        // =========================

        if (currentState != Button::buttonState) {

            Button::buttonState = currentState;


            // =========================
            // BUTTON PRESSED
            // =========================

            if (Button::buttonState == LOW) {

                Button::pressStartTime = currentTime;

                Button::longPressDetected = false;

                // Second click
                if (Button::waitingForSecondClick) {

                    Button::secondClickInProgress = true;

                } else {

                    Button::secondClickInProgress = false;
                }
            }


            // =========================
            // BUTTON RELEASED
            // =========================

            else {

                unsigned long pressDuration = currentTime - Button::pressStartTime;


                // If this was a long press,
                // LONG_PRESS has already been sent.

                if (Button::longPressDetected) {

                    Button::longPressDetected = false;

                    Button::waitingForSecondClick = false;

                    return BUTTON_RELEASED;
                }


                // =========================
                // SECOND CLICK
                // =========================

                if (Button::secondClickInProgress) {

                    Button::secondClickInProgress = false;

                    Button::waitingForSecondClick = false;

                    return DOUBLE_CLICK;
                }


                // =========================
                // NORMAL SHORT CLICK
                // =========================

                if (pressDuration < Button::LONG_PRESS_TIME) {

                    Button::waitingForSecondClick = true;

                    Button::lastClickTime = currentTime;
                }
            }
        }
    }


    // =========================
    // LONG PRESS
    // =========================
    //
    // This fires ONCE after the
    // button has been held for
    // LONG_PRESS_TIME.
    //
    // The button is still LOW.
    //
    // =========================

    if (Button::buttonState == LOW && !Button::longPressDetected && !Button::secondClickInProgress && !Button::waitingForSecondClick && (currentTime - Button::pressStartTime) >= Button::LONG_PRESS_TIME) {

        Button::longPressDetected = true;

        Serial.println("Long press started");

        return LONG_PRESS;
    }


    // =========================
    // Single click timeout
    // =========================

    if (Button::waitingForSecondClick && !Button::secondClickInProgress && (currentTime - Button::lastClickTime) > Button::DOUBLE_CLICK_TIME) {
        
        Button::waitingForSecondClick = false;

        return SINGLE_CLICK;
    }


    // Save raw state
    Button::lastButtonState = currentState;


    return NO_EVENT;
}


// =========================
// Check Button
// =========================

void checkButtonState() {

    ButtonEvent event = updateButton();

    (void)event;
}


// =========================
// Is Button Pressed
// =========================

bool isButtonPressed() {

    return digitalRead(bttnPin) == LOW;
}