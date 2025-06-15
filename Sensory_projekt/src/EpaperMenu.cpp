#include "EpaperMenu.h"
#include <Fonts/FreeMonoBold12pt7b.h>  // większa czcionka

#define BUTTON_DOWN 25
#define BUTTON_OK   26
#define WAKEUP_OPTIONS 4
RTC_DATA_ATTR int wakeupSelection = 3; // np. indeks wyboru z tablicy {10, 30, 60}
const int wakeupOptions[WAKEUP_OPTIONS] = {10, 30, 60, 300};
EpaperMenu::EpaperMenu()
  : display(GxEPD2_154_D67(/*CS=*/ 5, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)),
    currentState(SHOW_BMP280), selectedOption(0),
    lastDebounceTimeDown(0), lastDebounceTimeOk(0) {}

void EpaperMenu::begin() {
  Wire.begin(21, 22);
  display.init();
  display.setRotation(1);

  bmp.begin(0x77); // domyślny adres BMP280
  aht.begin();

  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_OK, INPUT_PULLUP);

  drawMenu();
}

void EpaperMenu::update() {
  handleButtonPress();
}

void EpaperMenu::handleButtonPress() {
  if (wasButtonDownPressed()) {
    selectedOption = (selectedOption + 1) % 4;
    drawMenu();
  } else if (wasButtonOkPressed()) {
    currentState = static_cast<MenuState>(selectedOption);
    switch (currentState) {
      case SHOW_BMP280: drawBMP280Data(); break;
      case SHOW_AHT20: drawAHT20Data(); break;
      case SET_WAKEUP: drawWakeupMenu(); break;
      case SHOW_SYSTEM: drawSystemInfo(); break;
    }
  }
}

void EpaperMenu::drawMenu() {
  display.setFont(&FreeMonoBold12pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.firstPage();
  do {
    display.setCursor(10, 30);
    display.println(selectedOption == 0 ? "> BMP280" : "  BMP280");
    display.setCursor(10, 60);
    display.println(selectedOption == 1 ? "> AHT20" : "  AHT20");
    display.setCursor(10, 90);
    display.println(selectedOption == 2 ? "> Wakeup" : "  Wakeup");
    display.setCursor(10, 120);
    display.println(selectedOption == 3 ? "> System Info" : "  System Info");
  } while (display.nextPage());
}

void EpaperMenu::drawBMP280Data() {
  display.setFont(&FreeMonoBold12pt7b);
  display.firstPage();
  do {
    display.setCursor(10, 40);
    display.print("Temp: "); display.print(bmp.readTemperature()); display.println(" C");

    display.setCursor(10, 80);
    display.print("Pressure: "); display.print(bmp.readPressure() / 100.0F); display.println(" hPa");
  } while (display.nextPage());
}

void EpaperMenu::drawAHT20Data() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  display.setFont(&FreeMonoBold12pt7b);
  display.firstPage();
  do {
    display.setCursor(10, 40);
    display.print("Temp: "); display.print(temp.temperature); display.println(" C");

    display.setCursor(10, 80);
    display.print("Humidity: "); display.print(humidity.relative_humidity); display.println(" %");
  } while (display.nextPage());
}

void EpaperMenu::drawWakeupMenu() {
  display.setFont(&FreeMonoBold12pt7b);

  unsigned long menuStart = millis();
  bool confirmed = false;

  int lastSelection = -1;  // Zmienna do śledzenia zmian
  bool screenNeedsUpdate = true;

  while (millis() - menuStart < 30000 && !confirmed) {
    if (screenNeedsUpdate) {
      display.firstPage();
      do {
        display.setCursor(10, 30);
        display.println("Wakeup time:");

        for (int i = 0; i < WAKEUP_OPTIONS; i++) {
          display.setCursor(10, 60 + i * 30);
          display.print((i == wakeupSelection) ? "> " : "  ");
          display.print(wakeupOptions[i]);
          display.println(" s");
        }
      } while (display.nextPage());

      screenNeedsUpdate = false;
    }

    if (wasButtonDownPressed()) {
      wakeupSelection = (wakeupSelection + 1) % WAKEUP_OPTIONS;
      menuStart = millis();
      screenNeedsUpdate = true;
    } else if (wasButtonOkPressed()) {
      confirmed = true;
    }

    delay(100);
  }

  // Wyświetl potwierdzenie tylko raz
  display.firstPage();
  do {
    display.setCursor(10, 80);
    display.print("Wybrane: ");
    display.print(wakeupOptions[wakeupSelection]);
    display.println(" s");
  } while (display.nextPage());

  delay(1000);
}



void EpaperMenu::drawSystemInfo() {
  display.setFont(&FreeMonoBold12pt7b);
  display.firstPage();
  do {
    display.setCursor(10, 60);
    display.print("Uptime: "); display.print(millis()/1000); display.println(" s");
    display.setCursor(10, 100);
    display.print("Wake up time: "); display.print(wakeupOptions[wakeupSelection]); display.println(" s");
  } while (display.nextPage());
}

void EpaperMenu::displaySleepNotice() {
  display.setFont(&FreeMonoBold12pt7b);
  display.firstPage();
  do {
    display.setCursor(10, 80);
    display.println("SLEEPING ZZZ");
  } while (display.nextPage());
}

bool EpaperMenu::wasButtonDownPressed() {
  static bool lastState = HIGH;
  bool currentState = digitalRead(BUTTON_DOWN);
  if (lastState == HIGH && currentState == LOW && millis() - lastDebounceTimeDown > 200) {
    lastDebounceTimeDown = millis();
    lastState = currentState;
    return true;
  }
  lastState = currentState;
  return false;
}

bool EpaperMenu::wasButtonOkPressed() {
  static bool lastState = HIGH;
  bool currentState = digitalRead(BUTTON_OK);
  if (lastState == HIGH && currentState == LOW && millis() - lastDebounceTimeOk > 200) {
    lastDebounceTimeOk = millis();
    lastState = currentState;
    return true;
  }
  lastState = currentState;
  return false;
}

int EpaperMenu::getSelectedWakeupTime() {
  return wakeupOptions[wakeupSelection];
}
