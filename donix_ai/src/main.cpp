#include <Arduino.h>
#include "rgb_led.h"
#include "bttn.h"
#include "oled.h"
#include "mic.h"
#include "amp.h"
#include "app_web_server.h"
#include "wifi_manager.h"


uint8_t ledDisplay = 0b00000000;

void setup() {

    Serial.begin(115200);

    setupButton();
    setupleds();

    for(int i = 0; i < 8; i++) {
        ledDisplay = ledDisplay | (1 << i);
        send8Bits(ledDisplay);
    }

    for(int i = 7; i >= 0; i--) {
        ledDisplay = ledDisplay & ~(1 << i);
        send8Bits(ledDisplay);
    }

    send8Bits(ledDisplay);

    setupOLED();
    mic_init();
    amp_init();

    setupWiFi();

    setupWebSocket();
}

void loop() {

    loopWebSocket();


    // =========================
    // Check Button
    // =========================

    ButtonEvent event = updateButton();


    // =========================
    // START RECORDING
    // =========================

    if (event == LONG_PRESS) {

        Serial.println("Recording started...");

        startRecording();
    }


    // =========================
    // RECORD WHILE HELD
    // =========================

    if (recording) {

        size_t size = 1024;

        mic_read(micBuffer, &size);

        if (size > 0) {

            for (size_t i = 0; i < size; i++) {

                // INMP441:
                // 24-bit audio stored in the upper
                // 24 bits of the 32-bit I2S frame.
                int16_t pcm = (int16_t)(micBuffer[i] >> 16);

                pcmBuffer[i] = pcm;
            }
            sendAudioBIN(
                (uint8_t*)pcmBuffer,
                size * sizeof(int16_t)
            );
        }
    }


    // =========================
    // BUTTON RELEASED
    // =========================

    if (event == BUTTON_RELEASED) {

        Serial.println("Button released.");

        stopRecording();
    }


    // =========================
    // OTHER BUTTON EVENTS
    // =========================

    if (event == SINGLE_CLICK) {

        Serial.println("Single Click Detected");

        // your LED code here
    }


    if (event == DOUBLE_CLICK) {

        Serial.println("Double Click Detected");

        ledDisplay = ledDisplay ^ 0b00000010;

        send8Bits(ledDisplay);
    }


    roboEyesUpdate();
}