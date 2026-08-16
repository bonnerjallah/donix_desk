#pragma once

enum ButtonEvent {
    NO_EVENT,
    SINGLE_CLICK,
    DOUBLE_CLICK,
    LONG_PRESS,
    DOUBLE_CLICK_LONG_PRESS
};

void setupButton();
ButtonEvent updateButton();
bool isButtonPressed();