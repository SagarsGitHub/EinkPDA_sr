#include <pocketmage.h>

// Singleton instance for system class
static Pocketmage pm_sys;

// Wire function for system class
void wirePocketmage() {
  pm_sys.setPwrBtnEventFlag(&PWR_BTN_event);
  // Wire all globals used by Pocketmage
  pm_sys.setNoSd(&noSD);
  pm_sys.setSdActive(&SDActive);
  pm_sys.setEditingFile(&editingFile);
  pm_sys.setKeypad(&keypad);
  pm_sys.setPrefs(&prefs);
  pm_sys.setTimeoutMillis(&timeoutMillis);
  pm_sys.setPrevTimeMillis(&prevTimeMillis);
  pm_sys.setDisableTimeout(&disableTimeout);
  pm_sys.setOledPowerSave(&OLEDPowerSave);
  pm_sys.setNewState(&newState);
  pm_sys.setCurrentAppState(&CurrentAppState);
  pm_sys.setCurrentHOMEState(&CurrentHOMEState);
  pm_sys.setCurrentKBState(&CurrentKBState);
  pm_sys.setDynamicScroll(&dynamicScroll);
  pm_sys.setNewLineAdded(&newLineAdded);
  pm_sys.setBattState(&battState);
  pm_sys.setPrevSec(&prevSec);
  pm_sys.setEinkTaskHandle(&einkHandlerTaskHandle);
}

// Accessor and ISR forwarder
Pocketmage& pocketmage() { return pm_sys; }
void IRAM_ATTR PWR_BTN_irq_handler() { pm_sys.PWR_BTN_irq(); }

void setupSystem(){
  // Serial, I2C, SPI
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
  // Wire system component and attach ISR similar to KB handler pattern
  wirePocketmage();
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
