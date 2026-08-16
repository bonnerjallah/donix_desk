#pragma once

void setupWebSocket();
void webSocketSendBIN(uint8_t* payload, size_t length);
void loopWebSocket();