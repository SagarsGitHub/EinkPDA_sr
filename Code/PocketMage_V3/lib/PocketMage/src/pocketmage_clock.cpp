//  888888ba  d888888P  a88888b. //
//  88    `8b    88    d8'   `88 //
// a88aaaa8P'    88    88        //
//  88   `8b.    88    88        //
//  88     88    88    Y8.   .88 //
//  dP     dP    dP     Y88888P' //

#include "pocketmage.h"

static constexpr const char* tag = "CLOCK";

RTC_PCF8563 rtc;

// Initialization of clock class
static PocketmageCLOCK pm_clock(rtc);

// Setup for Clock Class
void setupClock(){
  pinMode(RTC_INT, INPUT);
  if (!CLOCK().begin()) {
    ESP_LOGE(tag, "Couldn't find RTC");
    delay(1000);
  }
  // SET CLOCK IF NEEDED
  if (SET_CLOCK_ON_UPLOAD || CLOCK().getRTC().lostPower()) {
    CLOCK().setToCompileTimeUTC();
  }
  CLOCK().getRTC().start();
  wireClock();
}

// Wire function  for Clock class
// add any global references here + add set function to class header file
void wireClock(){
    
}

// Access for other apps
PocketmageCLOCK& CLOCK() { return pm_clock; }

bool PocketmageCLOCK::begin() {
  if (!rtc_.begin()) { begun_ = false; return false; }
  begun_ = true;
  return true;
}

bool PocketmageCLOCK::isValid() {
  if (!begun_) return false;
  DateTime t = rtc_.now();
  const bool saneYear = t.year() >= 2020 && t.year() < 2099;  // check for reasonable year for DateTime t
  return saneYear;
}

