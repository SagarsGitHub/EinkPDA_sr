//    .oooooo.   ooooo        oooooooooooo oooooooooo.    //
//   d8P'  `Y8b  `888'        `888'     `8 `888'   `Y8b   //
//  888      888  888          888          888      888  //
//  888      888  888          888oooo8     888      888  //
//  888      888  888          888    "     888      888  //
//  `88b    d88'  888       o  888       o  888     d88'  //
//   `Y8bood8P'  o888ooooood8 o888ooooood8 o888bood8P'    //     

#pragma once
#include <Arduino.h>
#include <U8g2lib.h>
#include <RTClib.h>
#include <vector>
#include <functional>
#include <utility>
#pragma region fonts
#pragma endregion

// To Do: add forward declaration of u8g2, remove #include<U8g2lib.h>, add forward declaration of pocketmage_clock and replace rtc w/ CLOCK()

// ===================== OLED CLASS =====================
class PocketmageOled {
public:
  explicit PocketmageOled(U8G2 &u8) : u8g2_(u8) {}

 // Wire up external buffers/state used to read from globals
  using MeasureTextFn = std::function<uint16_t(const String&)>; // returns text width in e-ink pixels
  using KbStateFn = std::function<int()>;
  using MaxCharsFn = std::function<uint16_t()>;

  void setReferenceWidth(uint16_t w)                                   { refWidth_ = w;}  // E-ink measurement
  void setMeasureTextWidth(MeasureTextFn fn)                { measure_ = std::move(fn);}  // function for measuring text width in e-ink pixels
  void setBattery(volatile int* st, const uint8_t* const* icons, int iconCount) {         // Battery icon/state
    battState_ = st;
    battIcons_ = icons;
    battIconCount_ = iconCount;
  }
  void setKeyboardState(int* kbState)                             { kbState_ = kbState;}      // Keyboard state: 0=NORMAL, 1=SHIFT, 2=FUNC
  void setKeyboardStateGetter(KbStateFn fn)               { kbStateFn_ = std::move(fn);}
  void setClock(RTC_PCF8563* rtc, bool* systemClock, bool* showYear, const char (*days)[12])   
  {rtc_ = rtc; systemClock_ = systemClock; showYear_ = showYear; days_ = days;}               // Clock
  void setMSC(bool* mscEnabled)                             { mscEnabled_ = mscEnabled;}      // Flags
  void setSD(volatile bool* sdActive)                           { sdActive_ = sdActive;}
  void setScrollBitmap(const uint8_t* bmp128x32)              { scrollBmp_ = bmp128x32;}
  void setMaxCharsPerLineEinkGetter(MaxCharsFn fn)       { maxCharsFn_ = std::move(fn);}
  void setPowerSave(int in)                                   { u8g2_.setPowerSave(in);}
  
  // Main methods
  void oledWord(String word, bool allowLarge = false, bool showInfo = true);
  void oledLine(String line, bool doProgressBar = true, String bottomMsg = "");
  void oledScroll();
  void infoBar();

private:
  U8G2                  &u8g2_;        // class reference to hardware oled object

  volatile int*         battState_     = nullptr;
  const uint8_t* const* battIcons_     = nullptr;
  int                   battIconCount_ = 0;

  int*                  kbState_       = nullptr;
  KbStateFn             kbStateFn_;

  RTC_PCF8563*          rtc_           = nullptr;
  bool*                 systemClock_   = nullptr;
  bool*                 showYear_      = nullptr;
  const char            (*days_)[12]   = nullptr;

  bool*                 mscEnabled_    = nullptr;
  volatile bool*        sdActive_      = nullptr;

  const uint8_t*        scrollBmp_     = nullptr;

  uint16_t              refWidth_      = 320;   
  MeasureTextFn         measure_;      // measure of display text width in e-ink pixels
  MaxCharsFn            maxCharsFn_;   // measure   
  // helpers
  uint16_t strWidth(const String& s) const;
};

void wireOled();
void setupOled();
PocketmageOled& OLED();