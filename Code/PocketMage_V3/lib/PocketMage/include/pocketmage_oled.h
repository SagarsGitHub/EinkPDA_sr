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

  void setPowerSave(int in)                                   { u8g2_.setPowerSave(in);}
  
  // Main methods
  void oledWord(String word, bool allowLarge = false, bool showInfo = true);
  void oledLine(String line, bool doProgressBar = true, String bottomMsg = "");
  void oledScroll();
  void infoBar();

private:
  U8G2                  &u8g2_;        // class reference to hardware oled object
 
  // helpers
  uint16_t strWidth(const String& s) const;
};

void setupOled();
PocketmageOled& OLED();