// dP     dP  88888888b dP    dP  888888ba   .88888.   .d888888   888888ba  888888ba  //
// 88   .d8'  88        Y8.  .8P  88    `8b d8'   `8b d8'    88   88    `8b 88    `8b //
// 88aaa8P'  a88aaaa     Y8aa8P  a88aaaa8P' 88     88 88aaaaa88a a88aaaa8P' 88     88 //
// 88   `8b.  88           88     88   `8b. 88     88 88     88   88   `8b. 88     88 //
// 88     88  88           88     88    .88 Y8.   .8P 88     88   88     88 88    .8P //
// dP     dP  88888888P    dP     88888888P  `8888P'  88     88   dP     dP 8888888P  //

#pragma once
#include <Arduino.h>
#include <Adafruit_TCA8418.h>

extern Adafruit_TCA8418 keypad;

// ===================== KB CLASS =====================
class PocketmageKB {
public:
  volatile bool TCA8418_event_ = false;  // Keypad interrupt event
  explicit PocketmageKB(Adafruit_TCA8418 &kp) : keypad_(kp) {}

  using KbStateFn = std::function<int()>;

  void setKeyboardState(int kbState)                       { kbState_ = kbState;}
  int getKeyboardState() const                               { return kbState_; }
  // Main methods
  char updateKeypress();
  void checkUSBKB();
  void disableInterrupts()                           { keypad_.disableInterrupts(); }
  void enableInterrupts()                             { keypad_.enableInterrupts(); }
  void flush()                                                   { keypad_.flush(); }
  void setTCA8418Event()                              {      TCA8418_event_ = true; }

private:
  Adafruit_TCA8418      &keypad_; // class reference to hardware keypad object
  int                   kbState_        = 0;

  volatile int*         prevTimeMillis_ = nullptr;
};

void wireKB();
void setupKB(int kb_irq_pin);
// Interrupt handler stored in IRAM for fast interrupt response
PocketmageKB& KB();