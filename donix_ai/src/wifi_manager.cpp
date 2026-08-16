#include <Arduino.h>
#include <WiFi.h>

#include "wifi_manager.h"
#include "secrets.h"

void setupWiFi() {
    
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.disconnect(true);

    delay(1000);

    Serial.println("Scanning for available networks...");

    int networksFound = WiFi.scanNetworks();

    const char* bestSSID = nullptr;
    const char* knownPassword = nullptr;

    int bestRSSI = -1000;

    for (int i = 0; i < networksFound; ++i) {
        String foundSSID = WiFi.SSID(i);
        int32_t foundRSSI = WiFi.RSSI(i);

        Serial.print("Found network: ");
        Serial.print(foundSSID);
        Serial.print(" | RSSI: ");
        Serial.println(foundRSSI);

        for (KnownNetwork network : knownNetworks) {
            if (foundSSID == network.ssid &&
                foundRSSI > bestRSSI)
            {
                bestSSID = network.ssid;
                knownPassword = network.password;
                bestRSSI = foundRSSI;
            }
        }
    }

    if (bestSSID == nullptr) {
        Serial.println("No known networks found.");
        return;
    }

    Serial.printf("Connecting to: %s | RSSI: %d\n", bestSSID, bestRSSI);

    WiFi.begin(bestSSID, knownPassword);

    unsigned long startTime = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 30000) {
        Serial.print("Status: ");
        Serial.println(WiFi.status());

        delay(100);
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi connection failed.");
        return;
    }

    Serial.printf("\nConnected to WiFi: %s\n", bestSSID);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}