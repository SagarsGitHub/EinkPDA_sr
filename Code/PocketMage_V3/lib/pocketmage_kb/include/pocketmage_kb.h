// dP     dP  88888888b dP    dP  888888ba   .88888.   .d888888   888888ba  888888ba  //
// 88   .d8'  88        Y8.  .8P  88    `8b d8'   `8b d8'    88   88    `8b 88    `8b //
// 88aaa8P'  a88aaaa     Y8aa8P  a88aaaa8P' 88     88 88aaaaa88a a88aaaa8P' 88     88 //
// 88   `8b.  88           88     88   `8b. 88     88 88     88   88   `8b. 88     88 //
// 88     88  88           88     88    .88 Y8.   .8P 88     88   88     88 88    .8P //
// dP     dP  88888888P    dP     88888888P  `8888P'  88     88   dP     dP 8888888P  //

#pragma once
#include <Arduino.h>
#include <utility>

// forward-declaration to avoid including Adafruit_TCA8418.h
class Adafruit_TCA8418;   

// ===================== KB CLASS =====================
class PocketmageKB {
public:
  explicit PocketmageKB(Adafruit_TCA8418 &kp) : keypad_(kp) {}

  // Main methods
  char updateKeypress();

  // keyboard state (0=NORMAL,1=SHIFT,2=FUNC)
  int  state() const                               { return kbStateInternal_; }
  void setState(int kbState)                    { kbStateInternal_ = kbState; }
  void setNormal()                                    { kbStateInternal_ = 0; }
  void toggleShift()    { kbStateInternal_ = (kbStateInternal_ == 1) ? 0 : 1; }
  void toggleFunc()     { kbStateInternal_ = (kbStateInternal_ == 2) ? 0 : 2; }
  
  // Wire up external buffers/state used to read from globals
  void setTCA8418EventFlag(volatile bool* TCA8418_event)          { TCA8418_event_ = TCA8418_event;}
  void setPrevTimeMillis(volatile int* prevTimeMillis)          { prevTimeMillis_ = prevTimeMillis;}


private:
  Adafruit_TCA8418      &keypad_; // class reference to hardware keypad object

  volatile bool*        TCA8418_event_  = nullptr;

  volatile int*         prevTimeMillis_ = nullptr;

  // Internal fallback/owned state
  int                   kbStateInternal_ = 0; // default NORMAL

  int currentKbState() const;
};

void wireKB();
void setupKB();
// Interrupt handler stored in IRAM for fast interrupt response
void IRAM_ATTR KB_irq_handler();
PocketmageKB& KB();
