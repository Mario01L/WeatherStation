#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_AHTX0.h>
#include "wifi_connect.h"
#include <WiFi.h>
#include "thingspeak_client.h"
#include "supabase_client.h"
#include "esp_sleep.h"
#include "config.h"
#include "EpaperMenu.h"

#define WAKEUP_BUTTON_PIN GPIO_NUM_27 // przycisk do wybudzania

//dane do połączenia z WiFi
const char* SSID = "";
const char* PASSWORD = "";

//tworzenie potrzebnych obiektów
Adafruit_BMP280 bmp;
Adafruit_AHTX0 aht20;
EpaperMenu menu;

void printdata();  // deklaracja printdata

void setup() {
  //inicjalizacja potrzebnych funkcji
  Serial.begin(9600);
  delay(1000);
  pinMode(WAKEUP_BUTTON_PIN, INPUT_PULLUP);

  if (!bmp.begin(0x77)) {
    Serial.println("Nie znaleziono BMP280!");
    while (1);
  }

   if (!aht20.begin()) {
     Serial.println("Nie znaleziono AHT20!");
     while (1);
   }


   // Konfiguracja wybudzania
  esp_sleep_enable_ext0_wakeup(WAKEUP_BUTTON_PIN, 0); // 0 = LOW
  // Obsługa przyczyny wybudzenia
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  
  if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER) {        //gdy wybudzenie timerem
    Serial.println("Wybudzono timerem!");
    printdata(); // wysyłanie danych
    disconnectWiFi();
  } else if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0) {    //gdy wybudzenie przyciskiem
    Serial.println("Wybudzono przyciskiem!"); 
    
  // Inicjalizacja menu i urządzeń
    menu.begin();

    unsigned long startTime = millis();
    unsigned long timeout = 60000; // 60 sekund na interakcję w menu

    while (millis() - startTime < timeout) {
      menu.update();
      delay(100);
    }
    menu.displaySleepNotice();
    
  } else {
    Serial.println("Pierwsze uruchomienie lub inna przyczyna.");
  }
  //ustawienie czasu wybudzania oraz wchodzenie w stan głębokiego uśpienia
  Serial.println("Koniec czasu — przejście w uśpienie.");
  esp_sleep_enable_timer_wakeup(menu.getSelectedWakeupTime() * 1000000LL);
  delay(100);
  esp_deep_sleep_start();
}

void loop() {
  // nie używane – ESP się resetuje po przebudzeniu
}

//wysyłanie danych z czujników przez Wifi
void printdata() {
  connectToWiFi(SSID, PASSWORD);

  if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Polaczono z WiFi!");
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());
  } else {
      Serial.println("Nie udaĹo siÄ poĹÄczyÄ z WiFi.");
  }

   sensors_event_t humidity, tempAHT;
   aht20.getEvent(&humidity, &tempAHT);

  // float temperature = tempAHT.temperature;
  float hum = humidity.relative_humidity;
  float temperature = bmp.readTemperature();
  //float hum = bmp.readHumidity();
  float pressure = bmp.readPressure() / 100.0F;
  float lightning = 0.0;

  Serial.printf("Temp: %.2f *C\nWilgotnoĹÄ: %.2f %%\nCiĹnienie: %.2f hPa\n", temperature, hum, pressure);

  sendToThingSpeak(temperature, pressure, hum, lightning);
  sendToSupabase(temperature, pressure, hum);
}