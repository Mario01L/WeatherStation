#include <WiFi.h>
#include <HTTPClient.h>

const char* THINGSPEAK_API_KEY = "API_KEY";
const char* THINGSPEAK_URL = "https://api.thingspeak.com/update";

void sendToThingSpeak(float temperature, float pressure, float humidity, float lightning) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Brak połączenia z Wi-Fi");
    return;
  }

  HTTPClient http;
  String url = String(THINGSPEAK_URL) +
               "?api_key=" + THINGSPEAK_API_KEY +
               "&field1=" + String(pressure, 2) +
               "&field2=" + String(humidity, 2) +
               "&field3=" + String(temperature, 2)+
               "&field4=" + String(lightning, 2);

  http.begin(url);
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    Serial.printf("Dane wysłane do ThingSpeak. Kod odpowiedzi: %d\n", httpResponseCode);
  } else {
    Serial.print("Błąd wysyłania: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}