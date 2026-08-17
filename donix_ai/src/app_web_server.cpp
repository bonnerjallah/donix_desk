#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "secrets.h"

WebSocketsClient webSocket;


// =========================
// WebSocket Send Binary Data
// =========================

void sendAudioBIN(uint8_t* data, size_t length) {

    Serial.print("WebSocket connected state: ");
    Serial.println(webSocket.isConnected() ? "YES" : "NO");

    webSocket.sendBIN(data, length);

    Serial.print("Sent binary: ");
    Serial.print(length);
    Serial.println(" bytes");
}

//=========================
// WebSocket Send Text Message
//=========================

void sendWebSocketText(const char* message) {
    webSocket.sendTXT(message);
}

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

            webSocket.sendTXT("HELLO");
            
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
// WebSocket Loop
// =========================

void loopWebSocket() {

    webSocket.loop();
}