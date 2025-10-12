#include <globals.h>

// Initialization of capacative touch class
static PocketmageTOUCH pm_touch(cap);

static constexpr const char* TAG = "TOUCH";

// Setup for Touch Class
void setupTouch(){
  // MPR121 / SLIDER
  if (!cap.begin(MPR121_ADDR)) {
    ESP_LOGE(TAG, "TouchPad Failed");
    OLED().oledWord("TouchPad Failed");
    delay(1000);
  }
  cap.setAutoconfig(true);
  wireTouch();
}

// Wire function for Touch class
// add any global references here + add set function to class header file
void wireTouch(){
  pm_touch.setAllLines(&allLines);
  pm_touch.setEink(EINK());
  pm_touch.setNewLineAdded(&newLineAdded);
}

// Access for other apps
PocketmageTOUCH& TOUCH() { return pm_touch; }