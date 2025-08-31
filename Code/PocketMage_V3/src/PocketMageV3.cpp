// PocketMage V3.0
// @Ashtf 2025

#include <pocketmage.h>

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
    case HOME:
      einkHandler_HOME();
      break;
    case TXT:
      einkHandler_TXT_NEW();
      break;
    case FILEWIZ:
      einkHandler_FILEWIZ();
      break;
    case TASKS:
      einkHandler_TASKS();
      break;
    case SETTINGS:
      einkHandler_settings();
      break;
    case USB_APP:
      einkHandler_USB();
      break;
    case CALENDAR:
      einkHandler_CALENDAR();
      break;
    case LEXICON:
      einkHandler_LEXICON();
      break;
    case JOURNAL:
      einkHandler_JOURNAL();
      break;
    // ADD APP CASES HERE
    default:
      einkHandler_HOME();
      break;
  }
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
// ADD PROCESS/KEYBOARD APP SCRIPTS HERE
void processKB() {
  switch (CurrentAppState) {
    case HOME:
      processKB_HOME();
      break;
    case TXT:
      processKB_TXT_NEW();
      break;
    case FILEWIZ:
      processKB_FILEWIZ();
      break;
    case TASKS:
      processKB_TASKS();
      break;
    case SETTINGS:
      processKB_settings();
      break;
    case USB_APP:
      processKB_USB();
      break;
    case CALENDAR:
      processKB_CALENDAR();
      break;
    case LEXICON:
      processKB_LEXICON();
      break;
    case JOURNAL:
      processKB_JOURNAL();
      break;
    // ADD APP CASES HERE
    default:
      processKB_HOME();
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
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);
  SPI.begin(SPI_SCK, -1, SPI_MOSI, -1);
  // setupSys() to begin here
  // OLED SETUP
  setupOled();

  // STARTUP JINGLE
  setupBZ();
  
  // WAKE INTERRUPT SETUP
  pinMode(KB_IRQ, INPUT);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_8, 0);

  // KEYBOARD SETUP
  setupKB();

  // EINK HANDLER SETUP
  setupEink();
  
  // SD CARD SETUP
  setupSD();


  
  // POWER SETUP
  pinMode(PWR_BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PWR_BTN), PWR_BTN_irq, FALLING);
  pinMode(CHRG_SENS, INPUT);
  pinMode(BAT_SENS, INPUT);
  //WiFi.mode(WIFI_OFF);
  //btStop();

  // SET CPU CLOCK FOR POWER SAVE MODE
  if (SAVE_POWER) setCpuFrequencyMhz(40 );
  else            setCpuFrequencyMhz(240);
  // setupCAP() to begin here
  // MPR121 / SLIDER
  if (!cap.begin(MPR121_ADDR)) {
    ESP_LOGE(TAG, "TouchPad Failed");
    OLED().oledWord("TouchPad Failed");
    delay(1000);
  }
  cap.setAutoconfig(true);

  // setupRTC() to begin here
  // RTC SETUP
  pinMode(RTC_INT, INPUT);
  if (!rtc.begin()) {
    ESP_LOGE(TAG, "Couldn't find RTC");
    delay(1000);
  }
  // SET CLOCK IF NEEDED
  if (SET_CLOCK_ON_UPLOAD || rtc.lostPower()) rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  rtc.start();

  // Set "random" seed
  randomSeed(analogRead(BAT_SENS));
  // end setupSys()
}

void loop() {
  if (!noTimeout)  checkTimeout();
  if (DEBUG_VERBOSE) printDebug();

  updateBattState();
  processKB();

  // Yield to watchdog
  vTaskDelay(50 / portTICK_PERIOD_MS);
  yield();
}
