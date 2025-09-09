// PocketMage V3.0
// @Ashtf 2025

#include <pocketmage.h>

static constexpr const char* TAG = "MAIN";

int x = 0;

// ADD PROCESS/KEYBOARD APP SCRIPTS HERE
void processKB() {
  
  // Draw a progress bar across the screen and then return to PocketMage OS
  u8g2.clearBuffer();
  u8g2.drawBox(0,0,x,u8g2.getDisplayHeight());
  
  x+=5;
  
  if (x > u8g2.getDisplayWidth()) {
    // Return to pocketMage OS
    rebootToPocketMage();
  }

  u8g2.sendBuffer();
  delay(10);
}

void applicationEinkHandler() {
  return;
}

/////////////////////////////////////////////////////////////
//  ooo        ooooo       .o.       ooooo ooooo      ooo  //
//  `88.       .888'      .888.      `888' `888b.     `8'  //
//   888b     d'888      .8"888.      888   8 `88b.    8   //
//   8 Y88. .P  888     .8' `888.     888   8   `88b.  8   //
//   8  `888'   888    .88ooo8888.    888   8     `88b.8   //
//   8    Y     888   .8'     `888.   888   8       `888   //
//  o8o        o888o o88o     o8888o o888o o8o        `8   //
/////////////////////////////////////////////////////////////
// SETUP
void setup() {
  PocketMage_INIT();
}

void loop() {
  // Check battery
  pocketmage::power::updateBattState();
  
  // Run KB loop
  processKB();

  // Yield to watchdog
  vTaskDelay(50 / portTICK_PERIOD_MS);
  yield();
}

// migrated from einkFunc.cpp
void einkHandler(void* parameter) {
  vTaskDelay(pdMS_TO_TICKS(250)); 
  for (;;) {
    applicationEinkHandler();

    vTaskDelay(pdMS_TO_TICKS(50));
    yield();
  }
}