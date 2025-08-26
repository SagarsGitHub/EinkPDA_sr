#include <pocketmage.h>
// To Do:
// make currentKBState a member of PocketmageKB and change all references in main apps/libraries

// Adafruit_TCA8418 keypad;
// Initialization of sd class
static PocketmageKB kb(keypad);

void IRAM_ATTR KB_irq_handler() { kb.TCA8418_irq(); }
// Setup for keyboard class
void setupKB() {
  if (!keypad.begin(TCA8418_DEFAULT_ADDR, &Wire)) {
    Serial.println("Error Initializing the Keyboard");
    OLED().oledWord("Keyboard INIT Failed");
    delay(1000);
    while (1);
  }
  keypad.matrix(4, 10);
  wireKB();
  attachInterrupt(digitalPinToInterrupt(KB_IRQ), KB_irq_handler, CHANGE); // potentially change to FALLING
  keypad.flush();
  keypad.enableInterrupts();
}

// Wire function for keyboard class
// add any global references here + add set function to class header file
void wireKB() {
    kb.setTCA8418EventFlag(&TCA8418_event);
    kb.setPrevTimeMillis(&prevTimeMillis);
    // lamda to avoid redundant functions To Do: make class interface for each pocketmage component in library
    kb.setKeyboardStateGetter([]{ return static_cast<int>(CurrentKBState); });
}


// Access for other apps
PocketmageKB& KB() { return kb; }
