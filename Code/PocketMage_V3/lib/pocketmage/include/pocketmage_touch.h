#pragma once
#include <Arduino.h>
#include <vector>
#include <FS.h>

class Adafruit_MPR121;   
class PocketmageEink;

// ===================== CAPACATIVE TOUCH CLASS =====================
class PocketmageTOUCH {
public:
  explicit PocketmageTOUCH(Adafruit_MPR121 &cap) : cap_(cap) {}

  // Wire up external buffers/state used to read from globals
  void setAllLines(std::vector<String>* allLines)                                           {allLines_ = allLines;}
  void setEink(PocketmageEink* eink)                                                               { eink_ = eink;}                // reference to pocketmage eink object
  void setEink(PocketmageEink& eink)                                                              { eink_ = &eink;}                // overloaded reference to pocketmage eink object
  void setNewLineAdded(volatile bool* newLineAdded)                                { newLineAdded_ = newLineAdded;}

  // Main methods
  void updateScrollFromTouch();
  bool updateScroll(int maxScroll, ulong& lineScroll);
  // getters 
  long int getDynamicScroll() const { return dynamicScroll_; }
  long int setDynamicScroll(long int val) { prev_dynamicScroll_ = dynamicScroll_; dynamicScroll_ = val; return dynamicScroll_; }
  long int getPrevDynamicScroll() const { return prev_dynamicScroll_; }
  int getLastTouch() const { return lastTouch_; }
  int getDiff() const { return dynamicScroll_ - prev_dynamicScroll_; }
private:
  Adafruit_MPR121      &cap_;                          // class reference to hardware touch object
  PocketmageEink*      eink_               = nullptr;
  std::vector<String>* allLines_           = nullptr;
  volatile bool*      newLineAdded_       = nullptr;
  volatile long int dynamicScroll_ = 0;         // Dynamic scroll offset
  volatile long int prev_dynamicScroll_ = 0;    // Previous scroll offset
  int lastTouch_ = -1;                          // Last touch event
  unsigned long lastTouchTime_ = 0;             // Last touch time
};

void wireTouch();
void setupTouch();
PocketmageTOUCH& TOUCH();