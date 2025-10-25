// PocketMage V3.0
// @Ashtf 2025

#include <globals.h>

static constexpr const char* TAG = "MAIN"; // TODO: Come up with a better tag

//        .o.       ooooooooo.   ooooooooo.    .oooooo..o  //
//       .888.      `888   `Y88. `888   `Y88. d8P'    `Y8  //
//      .8"888.      888   .d88'  888   .d88' Y88bo.       //
//     .8' `888.     888ooo88P'   888ooo88P'   `"Y8888o.   //
//    .88ooo8888.    888          888              `"Y88b  //
//   .8'     `888.   888          888         oo     .d8P  //
//  o88o     o8888o o888o        o888o        8""88888P'   //


// ADD E-INK HANDLER APP SCRIPTS HERE
void applicationEinkHandler() {
  switch (CurrentAppState) {
    // ADD APP CASES HERE
    default:
      break;
  }
}

// ADD PROCESS/KEYBOARD APP SCRIPTS HERE
void processKB() {
  switch (CurrentAppState) {
    // ADD APP CASES HERE
    default:
      break;
  }
}

void bootApp() {
  switch (CurrentAppState) {
    // ADD APP CASES HERE
    default:
      break;
  }
}
//  ooo        ooooo       .o.       ooooo ooooo      ooo  //
//  `88.       .888'      .888.      `888' `888b.     `8'  //
//   888b     d'888      .8"888.      888   8 `88b.    8   //
//   8 Y88. .P  888     .8' `888.     888   8   `88b.  8   //
//   8  `888'   888    .88ooo8888.    888   8     `88b.8   //
//   8    Y     888   .8'     `888.   888   8       `888   //
//  o8o        o888o o88o     o8888o o888o o8o        `8   //

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////|
// SETUP
void setup() {
  PocketMage_INIT();
}

// Keyboard / OLED Loop
void loop() {
  processKB();

  // Yield to watchdog
  vTaskDelay(50 / portTICK_PERIOD_MS);
  yield();
}

// E-Ink Loop
void einkHandler(void* parameter) {
  vTaskDelay(pdMS_TO_TICKS(250)); 
  for (;;) {
    applicationEinkHandler();

    vTaskDelay(pdMS_TO_TICKS(50));
    yield();
  }
}