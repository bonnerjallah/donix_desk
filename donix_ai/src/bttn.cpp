#include <Arduino.h>
#include "bttn.h"
#include <stdint.h>

const int bttnPin = 13;

namespace Button {

constexpr unsigned long DEBOUNCE_DELAY = 50;
constexpr unsigned long DOUBLE_CLICK_TIME = 400;
constexpr unsigned long LONG_PRESS_TIME = 1000;
constexpr unsigned long DOUBLE_CLICK_LONG_PRESS_TIME = 1500;

bool buttonState = HIGH;
bool lastButtonState = HIGH;

unsigned long lastDebounceTime = 0;
unsigned long lastClickTime = 0;
unsigned long pressStartTime = 0;

bool waitingForSecondClick = false;
bool secondClickInProgress = false;

bool longPressDetected = false;
bool doubleClickLongPressDetected = false;

}


// Initialize button
void setupButton() {

    pinMode(bttnPin, INPUT_PULLUP);

    Button::buttonState = digitalRead(bttnPin);
    Button::lastButtonState = Button::buttonState;

    Button::lastDebounceTime = millis();
}


// Check button and return an event
ButtonEvent updateButton() {

    bool currentState = digitalRead(bttnPin);
    unsigned long currentTime = millis();


    // --------------------------------
    // DETECT RAW STATE CHANGE
    // --------------------------------

    if (currentState != Button::lastButtonState) {
        Button::lastDebounceTime = currentTime;
    }


    // --------------------------------
    // WAIT FOR DEBOUNCE
    // --------------------------------

    if ((currentTime - Button::lastDebounceTime) >=
        Button::DEBOUNCE_DELAY) {


        // --------------------------------
        // STABLE STATE CHANGED
        // --------------------------------

        if (currentState != Button::buttonState) {

            Button::buttonState = currentState;


            // ==================================
            // BUTTON PRESSED
            // ==================================

            if (Button::buttonState == LOW) {

                Button::pressStartTime = currentTime;

                Button::longPressDetected = false;
                Button::doubleClickLongPressDetected = false;


                // If we already had the first click,
                // this is the second click.
                if (Button::waitingForSecondClick) {

                    Button::secondClickInProgress = true;

                }

            }


            // ==================================
            // BUTTON RELEASED
            // ==================================

            else {

                unsigned long pressDuration =
                    currentTime - Button::pressStartTime;


                // --------------------------------
                // SECOND CLICK WAS RELEASED
                // --------------------------------

                if (Button::secondClickInProgress) {

                    Button::secondClickInProgress = false;


                    // If it was held for 1.5 seconds,
                    // it is NOT a normal double click.
                    if (
                        pressDuration >=
                        Button::DOUBLE_CLICK_LONG_PRESS_TIME
                    ) {

                        Button::waitingForSecondClick = false;

                        return DOUBLE_CLICK_LONG_PRESS;
                    }


                    // Otherwise it was a normal
                    // double click.
                    Button::waitingForSecondClick = false;

                    return DOUBLE_CLICK;
                }


                // --------------------------------
                // FIRST CLICK RELEASED
                // --------------------------------

                if (pressDuration >= Button::LONG_PRESS_TIME) {

                    Button::longPressDetected = true;
                    Button::waitingForSecondClick = false;

                    return LONG_PRESS;
                }


                // --------------------------------
                // FIRST NORMAL CLICK
                // --------------------------------

                Button::waitingForSecondClick = true;
                Button::lastClickTime = currentTime;
            }
        }
    }


    // Save current raw state for next check
    Button::lastButtonState = currentState;


    // --------------------------------
    // DOUBLE CLICK TIMEOUT
    // --------------------------------

    if (
        Button::waitingForSecondClick &&
        !Button::secondClickInProgress &&
        (currentTime - Button::lastClickTime >
         Button::DOUBLE_CLICK_TIME)
    ) {

        Button::waitingForSecondClick = false;

        return SINGLE_CLICK;
    }


    return NO_EVENT;
}

void checkButtonState() {

    ButtonEvent event = updateButton();
}

bool isButtonPressed() {
    return digitalRead(bttnPin) == LOW;
}