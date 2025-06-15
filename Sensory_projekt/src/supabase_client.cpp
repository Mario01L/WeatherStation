#include "supabase_client.h"
#include <HTTPClient.h>
#include <WiFi.h>

const char* SUPABASE_URL = "https://encglewfzpvfboxsprqs.supabase.co/rest/v1/dane";
const char* SUPABASE_API_KEY = "SUPABASE_API_KEY"; 

void sendToSupabase(float temperature, float pressure, float humidity) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Brak Wi-Fi");
    return;
  }

  HTTPClient http;
  http.begin(SUPABASE_URL);
  http.addHeader("apikey", SUPABASE_API_KEY);
  http.addHeader("Authorization", String("Bearer ") + SUPABASE_API_KEY);
  http.addHeader("Content-Type", "application/json");

  String payload = "{";
  payload += "\"temperature\":" + String(temperature, 2) + ",";
  payload += "\"pressure\":" + String(pressure, 2) + ",";
  payload += "\"humidity\":" + String(humidity, 2);
  payload += "}";

  int code = http.POST(payload);

  if (code > 0) Serial.println("Supabase OK");
  else Serial.printf("Supabase FAIL: %d\n", code);

  http.end();
}
