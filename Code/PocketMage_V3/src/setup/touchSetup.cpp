#include <pocketmage.h>

// Initialization of capacative touch class
static PocketmageTOUCH pm_touch(cap);

void setupTouch(){
  if (!cap.begin(MPR121_ADDR)) {
    OLED().oledWord("TouchPad Failed");
    delay(1000);
  }
  cap.setAutoconfig(true);
  wireTouch();
}

void wireTouch(){
  pm_touch.setAllLines(&allLines);
  pm_touch.setEink(EINK());
  pm_touch.setNewLineAdded(&newLineAdded);
  pm_touch.setDynamicScroll(&dynamicScroll);
  pm_touch.setPrevDynamicScroll(&prev_dynamicScroll);
  pm_touch.setLastTouch(&lastTouch);
  pm_touch.setLastTouchTime(&lastTouchTime);
}

PocketmageTOUCH& TOUCH() { return pm_touch; }

