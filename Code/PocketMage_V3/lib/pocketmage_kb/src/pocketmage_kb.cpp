#include <pocketmage_kb.h>
#include <Adafruit_TCA8418.h>

char currentKB[4][10];            // Current keyboard layout

char keysArray[4][10] = {
    { 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p' },
    { 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',   8 },  // 8:BKSP
    {   9, 'z', 'x', 'c', 'v', 'b', 'n', 'm', '.',  13 },  // 9:TAB, 13:CR
    {   0,  17,  18, ' ', ' ', ' ',  19,  20,  21,   0 }   // 17:SHFT, 18:FN, 19:<-, 20:SEL, 21:->
};
char keysArraySHFT[4][10] = {
    { 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P' },
    { 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',   8 },
    {   9, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',',  13 },
    {   0,  17,  18, ' ', ' ', ' ',  19,  20,  21,   0 }
};
char keysArrayFN[4][10] = {
    { '1', '2', '3', '4', '5', '6', '7',  '8',  '9', '0' },
    { '#', '!', '$', ':', ';', '(', ')', '\'', '\"',   8 },
    {  14, '%', '_', '&', '+', '-', '/',  '?',  ',',  13 },
    {   0,  17,  18, ' ', ' ', ' ',  12,    7,    6,   0 }
};

int PocketmageKB::currentKbState() const {
  if (kbStateFn_) return kbStateFn_();
  if (kbState_)   return *kbState_;
  return 0;
}

void PocketmageKB::TCA8418_irq() {
  if (TCA8418_event_) *TCA8418_event_ = true;
}

char PocketmageKB::updateKeypress() {
  if ((TCA8418_event_) && (*TCA8418_event_ == true)) {
    int k = keypad_.getEvent();
    
    //  try to clear the IRQ flag
    //  if there are pending events it is not cleared
    keypad_.writeRegister(TCA8418_REG_INT_STAT, 1);
    int intstat = keypad_.readRegister(TCA8418_REG_INT_STAT);
    if ((intstat & 0x01) == 0) *TCA8418_event_ = false;

    if (k & 0x80) {   //Key pressed, not released
      k &= 0x7F;
      k--;
      //return currentKB[k/10][k%10];
      if ((k/10) < 4) {
        //Key was pressed, reset timeout counter
        if (prevTimeMillis_) *prevTimeMillis_ = millis();

        //Return Key
        switch (currentKbState()) {
          case 0:
            return keysArray[k/10][k%10];
          case 1:
            return keysArraySHFT[k/10][k%10];
          case 2:
            return keysArrayFN[k/10][k%10];
          default:
            return 0;
        }
      }
    }
  }

  return 0;

}