#include "wifi_connect.h"
#include <WiFi.h>

void connectToWiFi(const char* ssid, const char* password) {
  WiFi.begin(ssid, password);
  Serial.print("Łączenie z Wi-Fi");

   unsigned long startTime = millis();
  const unsigned long timeout = 15000; // 15 sekund

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - startTime > timeout) {
      Serial.println("\nPrzekroczono czas łączenia z Wi-Fi");
      return; // wyjście z funkcji, połączenie nieudane
    }
  }

  Serial.println("\nPołączono z Wi-Fi!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void disconnectWiFi() {
  WiFi.disconnect();
}