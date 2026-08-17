#pragma once

#include <Arduino.h>

enum ButtonEvent {
    NO_EVENT,
    SINGLE_CLICK,
    DOUBLE_CLICK,
    LONG_PRESS,
    DOUBLE_CLICK_LONG_PRESS,
    BUTTON_RELEASED
};

void setupButton();

ButtonEvent updateButton();

void checkButtonState();

bool isButtonPressed();