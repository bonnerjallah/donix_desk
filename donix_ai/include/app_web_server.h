#pragma once

void setupWebSocket();
void loopWebSocket();

void sendAudioBIN(uint8_t* data, size_t length);
void sendWebSocketText(const char* message);