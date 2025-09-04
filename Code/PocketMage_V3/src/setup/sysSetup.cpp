#include <pocketmage.h>
// sys
#include <pocketmage_sys.h>
// display
#include <U8g2lib.h>
#include <GxEPD2_BW.h>
#include <RTClib.h>

constexpr int kBattIconCount = sizeof(batt_allArray) / sizeof(batt_allArray[0]);

static constexpr const char* TAG = "SETUP"; // TODO: Come up with a better tag

// ===================== ISR =====================
void IRAM_ATTR PWR_BTN_irq_handler() { PWR_BTN_event = true; }
void IRAM_ATTR KB_irq_handler() { TCA8418_event = true; }

// ===================== CLOCK =====================
#pragma region CLOCK

void setupClock(){
  pinMode(RTC_INT, INPUT);
  if (!CLOCK().begin()) {
    ESP_LOGE(TAG, "Couldn't find RTC");
    delay(1000);
  }
  // SET CLOCK IF NEEDED
  if (SET_CLOCK_ON_UPLOAD || CLOCK().getRTC().lostPower()) {
    CLOCK().setToCompileTimeUTC();
  }
  CLOCK().getRTC().start();
}


// clock object reference for other apps
PocketmageCLOCK& CLOCK() {
  static PocketmageCLOCK pm_clock(rtc);
  return pm_clock;
}
#pragma endregion



// ===================== TOUCH =====================
#pragma region TOUCH

void setupTouch(){
  // MPR121 / SLIDER
  if (!cap.begin(MPR121_ADDR)) {
    ESP_LOGE(TAG, "TouchPad Failed");
    OLED().oledWord("TouchPad Failed");
    delay(1000);
  }
  cap.setAutoconfig(true);
  wireTouch();
}

void wireTouch(){
  auto pm_touch = TOUCH();
  pm_touch.setAllLines(&allLines);
  pm_touch.setEink(EINK());
  pm_touch.setNewLineAdded(&newLineAdded);
  pm_touch.setDynamicScroll(&dynamicScroll);
  pm_touch.setPrevDynamicScroll(&prev_dynamicScroll);
  pm_touch.setLastTouch(&lastTouch);
  pm_touch.setLastTouchTime(&lastTouchTime);
}

// touch object reference for other apps
PocketmageTOUCH& TOUCH() { 
  static PocketmageTOUCH touch(cap);
  return touch;
}
#pragma endregion



// ===================== SD =====================
#pragma region SD

// @ dependencies:
//   - setupOled()
//   - setupBZ()
//   - setupEINK()
void setupSD() {
  SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0);
  if (!SD_MMC.begin("/sdcard", true) || SD_MMC.cardType() == CARD_NONE) {
    ESP_LOGE(TAG, "MOUNT FAILED");

    OLED().oledWord("SD Card Not Detected!");
    delay(2000);
    if (ALLOW_NO_MICROSD) {
      OLED().oledWord("All Work Will Be Lost!");
      delay(5000);
      noSD = true;
    }
    else {
      OLED().oledWord("Insert SD Card and Reboot!");
      delay(5000);
      // Put OLED to sleep
      u8g2.setPowerSave(1);
      // Shut Down Jingle
      BZ().playJingle(Jingles::Shutdown);
      // Sleep
      esp_deep_sleep_start();
      return;
    }
  }
  wireSD();

  setCpuFrequencyMhz(240);
  // Create folders and files if needed
  if (!SD_MMC.exists("/sys"))     SD_MMC.mkdir("/sys");
  if (!SD_MMC.exists("/journal")) SD_MMC.mkdir("/journal");
  if (!SD_MMC.exists("/dict")) SD_MMC.mkdir("/dict");
  if (!SD_MMC.exists("/sys/events.txt")) {
    File f = SD_MMC.open("/sys/events.txt", FILE_WRITE);
    if (f) f.close();
  }
  if (!SD_MMC.exists("/sys/tasks.txt")) {
    File f = SD_MMC.open("/sys/tasks.txt", FILE_WRITE);
    if (f) f.close();
  }
  if (!SD_MMC.exists("/sys/SDMMC_META.txt")) {
    File f = SD_MMC.open("/sys/SDMMC_META.txt", FILE_WRITE);
    if (f) f.close();
  }
 
  pocketmage().loadState();

}

// Wire function  for SD class
// add any global references here + add set function to class header file
void wireSD() {
  auto pm_sd = SD();
  pm_sd.setFileSys(&SD_MMC);
  pm_sd.setOled(OLED());
  pm_sd.setEink(EINK());
  pm_sd.setEditingFile(&editingFile);
  pm_sd.setFilesList(filesList);
  pm_sd.setNoSD(&noSD);
  pm_sd.setNoTimeout(&noTimeout);
}

// Access for other apps
PocketmageSD& SD() { 
  static PocketmageSD sd;
  return sd;
 }
#pragma endregion



// ===================== EINK =====================
#pragma region EINK

// Setup for Eink Class
void setupEink() {
  wireEink();
  display.init(115200);
  display.setRotation(3);
  display.setTextColor(GxEPD_BLACK);
  display.setFullWindow();
  EINK().setTXTFont(&FreeMonoBold9pt7b); // default font, computeFontMetrics_()
  OLED().setReferenceWidth(display.width()); // set oled eink reference width after setting up eink
  xTaskCreatePinnedToCore(
    einkHandler,             // Function name
    "einkHandlerTask",       // Task name
    10000,                   // Stack size
    NULL,                    // Parameters 
    1,                       // Priority 
    &einkHandlerTaskHandle,  // Task handle
    0                        // Core ID 
  );

}

// Wire function  for Eink class
// add any global references here + add set function to class header file
void wireEink() {
  auto pm_eink = EINK();
  pm_eink.setTextBuffer(&allLines);
  pm_eink.setEditingFilePtr(&editingFile);
  pm_eink.setDynamicScroll(&dynamicScroll);
}

// Access for other apps 
PocketmageEink& EINK() { 
  static PocketmageEink eink(display);
  return eink;
 }
#pragma endregion



// ===================== KEYBOARD =====================
#pragma region KEYBOARD

// Setup for keyboard class
void setupKB() {
  if (!keypad.begin(TCA8418_DEFAULT_ADDR, &Wire)) {
    ESP_LOGE(TAG, "Error Initializing the Keyboard");
    OLED().oledWord("Keyboard INIT Failed");
    delay(1000);
    while (1);
  }
  keypad.matrix(4, 10);
  wireKB();
  attachInterrupt(digitalPinToInterrupt(KB_IRQ), KB_irq_handler, FALLING);
  keypad.flush();
  keypad.enableInterrupts();
}

// Wire function for keyboard class
// add any global references here + add set function to class header file
void wireKB() {
  auto pm_kb = KB();
  pm_kb.setTCA8418EventFlag(&TCA8418_event);
  pm_kb.setPrevTimeMillis(&prevTimeMillis);
  pm_kb.setState(NORMAL);
}

// Access for other apps
PocketmageKB& KB() { 
  static PocketmageKB kb(keypad);
  return kb;
 }
#pragma endregion



// ===================== BUZZER =====================
#pragma region BUZZER

// Setup for Buzzer Class
void setupBZ() {
  BZ().playJingle(Jingles::Startup);
}

// Wire function  for Buzzer class
// add any global references here + add set function to class header file
void wireBZ() {
}

// Access for other apps
inline PocketmageBZ& BZ() {
  static PocketmageBZ bz(buzzer);
  return bz;
}
#pragma endregion




// ===================== OLED =====================
#pragma region OLED

// e-ink text width wrapper to work with Eink display
static uint16_t einkMeasureWidth(const String& s) {
  int16_t x1, y1; uint16_t w, h;
  display.getTextBounds(s, 0, 0, &x1, &y1, &w, &h);
  return w;
}

// Setup for Oled Class
void setupOled() {
  u8g2.begin();
  u8g2.setBusClock(10000000);
  u8g2.setPowerSave(0);
  u8g2.clearBuffer();
  u8g2.sendBuffer();
  wireOled();

  // SHOW "PocketMage" while DEVICE BOOTS
  OLED().oledWord("   PocketMage   ", true, false);
}

// Wire function  for Oled class
// add any global references here + add set function to class header file
void wireOled() {
  auto& pm_oled = OLED();
  pm_oled.setAllLines(&allLines);
  pm_oled.setDynamicScroll(&dynamicScroll);
  pm_oled.setBattery(&battState, batt_allArray, kBattIconCount);
  pm_oled.setMSC(&mscEnabled);
  pm_oled.setSD(&SDActive);
  pm_oled.setScrollBitmap(scrolloled0);
  pm_oled.setMeasureTextWidth(einkMeasureWidth);
  pm_oled.setMaxCharsPerLineEinkGetter([]{ return EINK().maxCharsPerLine(); });
  pm_oled.setClock(&CLOCK().getRTC(), &SYSTEM_CLOCK, &SHOW_YEAR, daysOfTheWeek);
}

// oled object reference for other apps
inline PocketmageOled& OLED() {
  static PocketmageOled oled(u8g2);
  return oled;
}
#pragma endregion



// ===================== SETUP =====================
#pragma region SETUP

void setupSystem(){
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);
  SPI.begin(SPI_SCK, -1, SPI_MOSI, -1);
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
  attachInterrupt(digitalPinToInterrupt(PWR_BTN), PWR_BTN_irq_handler, FALLING);
  pinMode(CHRG_SENS, INPUT);
  pinMode(BAT_SENS, INPUT);
  //WiFi.mode(WIFI_OFF);
  //btStop();

  // SET CPU CLOCK FOR POWER SAVE MODE
  if (SAVE_POWER) setCpuFrequencyMhz(40 );
  else            setCpuFrequencyMhz(240);
  
  // CAPACATIVE TOUCH SETUP
  setupTouch();

  // RTC SETUP
  setupClock();

  // Set "random" seed
  randomSeed(analogRead(BAT_SENS));
}
// Access for other apps
Pocketmage& pocketmage() { 
  static Pocketmage pm;
  return pm;  
}
#pragma endregion
