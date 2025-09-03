#pragma once
#include <Arduino.h>
#include <RTClib.h>


class PocketmageCLOCK {
public:
  explicit PocketmageCLOCK(RTC_PCF8563 &rtc) : rtc_(rtc) {}

  // Main class methods
  void setToCompileTimeUTC() { rtc_.adjust(DateTime(F(__DATE__), F(__TIME__))); }
  DateTime nowDT()                                         { return rtc_.now(); }

  bool begin();
  bool isValid();

  // Getters
  RTC_PCF8563& getRTC()                                          { return rtc_; }

private:
  RTC_PCF8563  &rtc_;  
  bool         begun_   = false;
};

void setupClock();
PocketmageCLOCK& CLOCK();
