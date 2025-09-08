#include <pocketmage.h>

static constexpr const char* TAG = "CLOCK"; // TODO: Come up with a better tag

// Initialization of clock class
static PocketmageCLOCK pm_clock(rtc);

// Setup for Clock Class
void setupClock(){
  pinMode(RTC_INT, INPUT);
  if (!CLOCK().begin()) {
    ESP_LOGE(TAG, "Couldn't find RTC");
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