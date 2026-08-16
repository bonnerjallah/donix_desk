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

    ButtonEvent event = updateButton();

    if (event == SINGLE_CLICK) {

        Serial.println("Single Click Detected");
        Serial.println("Turning LEDs ON");

        for (int i = 0; i < 8; i++) {
            ledDisplay = ledDisplay | (1 << i);
            send8Bits(ledDisplay);
            delay(100);
        }

        for (int i = 7; i >= 0; i--) {
            ledDisplay = ledDisplay & ~(1 << i);
            send8Bits(ledDisplay);
            delay(100);
        }

    } else if (event == DOUBLE_CLICK) {

        Serial.println("Double Click Detected");

        ledDisplay = ledDisplay ^ 0b00000010;
        send8Bits(ledDisplay);

    } else if (event == LONG_PRESS) {

        Serial.println("Long Press Detected");

        int32_t micBuffer[1024];
        int16_t pcmBuffer[1024];

        size_t size = 1024;

        Serial.println("Calling mic_read()...");

        int result = mic_read(micBuffer, &size);


        Serial.print("mic_read result: ");
        Serial.println(result);

        Serial.print("Samples read: ");
        Serial.println(size);

        if (result == 0) {

            for (int i = 0; i < size; i++) {

    pcmBuffer[i] = (int16_t)(micBuffer[i] >> 8);

    Serial.println(pcmBuffer[i]);
}

            size_t bytesToSend = size * sizeof(int16_t);

            Serial.print("Sending ");
            Serial.print(bytesToSend);
            Serial.println(" bytes...");

            sendAudioBIN((uint8_t*)pcmBuffer, bytesToSend);

            Serial.println("sendBIN called");

            sendWebSocketText("RECORDING_COMPLETE");
        }
    }
    

    roboEyesUpdate();

}
