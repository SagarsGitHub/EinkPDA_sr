//  888888ba  d888888P  a88888b. //
//  88    `8b    88    d8'   `88 //
// a88aaaa8P'    88    88        //
//  88   `8b.    88    88        //
//  88     88    88    Y8.   .88 //
//  dP     dP    dP     Y88888P' //

#pragma once
#include <Arduino.h>
#include <RTClib.h>


class PocketmageCLOCK {
public:
  explicit PocketmageCLOCK(RTC_PCF8563 &rtc) : rtc_(rtc) {}

  bool begin();
  bool isValid();

  void setToCompileTimeUTC() { rtc_.adjust(DateTime(F(__DATE__), F(__TIME__))); }

  DateTime nowDT()                                         { return rtc_.now(); }
  RTC_PCF8563& getRTC()                                          { return rtc_; }

private:
  RTC_PCF8563 &rtc_;  
  bool begun_ = false;
};

void wireClock();
void setupClock();
PocketmageCLOCK& CLOCK();
