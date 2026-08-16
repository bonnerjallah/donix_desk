#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "secrets.h"

WebSocketsClient webSocket;

// Function declaration
void webSocketSendBIN(uint8_t* payload, size_t length);


// =========================
// WebSocket Event Handler
// =========================

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {

    switch (type)
    {
        case WStype_DISCONNECTED:

            Serial.println("WebSocket disconnected");

            break;


        case WStype_CONNECTED:

            Serial.println("WebSocket connected");

            webSocketSendBIN(
                (uint8_t*)"Hello from ESP32",
                strlen("Hello from ESP32")
            );

            break;


        case WStype_TEXT:

            Serial.println("Message received from server:");

            Serial.println((char*)payload);

            break;


        case WStype_BIN:

            Serial.println("Binary data received");

            break;


        default:

            break;
    }
}


// =========================
// WebSocket Setup
// =========================

void setupWebSocket() {

    webSocket.begin(
        NODE_SERVER_IP,
        3001,
        "/ws"
    );

    webSocket.onEvent(webSocketEvent);

    webSocket.setReconnectInterval(5000);

    Serial.println("WebSocket client started");
}


// =========================
// Send Binary Data
// =========================

void webSocketSendBIN(uint8_t* payload, size_t length) {

    webSocket.sendBIN(payload, length);
}


// =========================
// WebSocket Loop
// =========================

void loopWebSocket() {

    webSocket.loop();
}