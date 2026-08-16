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
        size_t size = sizeof(micBuffer) / sizeof(micBuffer[0]);

        if (mic_read(micBuffer, &size) == 0){

            Serial.printf(
                "Sending %d samples (%d bytes) to Node.js\n",
                (int)size,
                (int)(size * sizeof(int32_t))
            );

            webSocketSendBIN(
                (uint8_t*)micBuffer,
                size * sizeof(int32_t)
            );
       }

    }
    

    roboEyesUpdate();

}
