#include "pocketmage_clock.h"


bool PocketmageCLOCK::begin() {
  if (!rtc_.begin()) { begun_ = false; return false; }
  begun_ = true;
  return true;
}


