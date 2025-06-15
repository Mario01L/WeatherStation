// EpaperMenu.h
#ifndef EPAPER_MENU_H
#define EPAPER_MENU_H

#include <GxEPD2_BW.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_AHTX0.h>


class EpaperMenu {
  public:
    EpaperMenu();
    void begin();             // Inicjalizacja urządzeń
    void update();            // Główna pętla aktualizująca menu
    void handleButtonPress(); // Obsługa przycisków
    void displaySleepNotice();
    int getSelectedWakeupTime();
    void deinit();
  private:
    // Stany menu
    enum MenuState {
      SHOW_BMP280,
      SHOW_AHT20,
      SET_WAKEUP,
      SHOW_SYSTEM
    };

    MenuState currentState;
    int selectedOption;

    // Rysowanie poszczególnych ekranów
    void drawMenu();
    void drawBMP280Data();
    void drawAHT20Data();
    void drawWakeupMenu();
    void drawSystemInfo();

    // Funkcja do zmiany czasu wybudzania
    void changeWakeupTime();

    // Obsługa przycisków (z debouncingiem)
    bool wasButtonDownPressed();
    bool wasButtonOkPressed();

    // Czujniki
    Adafruit_BMP280 bmp;
    Adafruit_AHTX0 aht;

    // E-paper display
    GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display;

    // Zmienne do debouncowania
    unsigned long lastDebounceTimeDown;
    unsigned long lastDebounceTimeOk;
};

#endif // EPAPER_MENU_H
