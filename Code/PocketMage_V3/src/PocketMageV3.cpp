// PocketMage V3.0
// @Ashtf 2025

#include <pocketmage.h>
#include <Preferences.h>
#include <time.h>
#include <sys/time.h>

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
    case CALENDAR:
      einkHandler_CALENDAR();
      break;
    case JOURNAL:
      einkHandler_JOURNAL();
      break;
    case LEXICON:
      einkHandler_LEXICON();
      break;
    case SETTINGS:
      einkHandler_settings();
      break;
    case CALC:
      einkHandler_CALC();
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
    case CALENDAR:
      processKB_CALENDAR();
      break;
    case JOURNAL:
      processKB_JOURNAL();
      break;
    case LEXICON:
      processKB_LEXICON();
      break;
    case SETTINGS:
      processKB_settings();
      break;
    case CALC:
      processKB_CALC();
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
  // setupSys() to begin here
  // OLED SETUP
  setupOled();

  Serial.println("setup oled!");
  // WAKE INTERRUPT SETUP
  pinMode(KB_IRQ, INPUT);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_8, 0);


  // KEYBOARD SETUP
  setupKB();
  Serial.println("setup kb!");
  // EINK HANDLER SETUP

  setupEink();
  Serial.println("setup eink!");



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
    Serial.println("TouchPad Failed");
    OLED().oledWord("TouchPad Failed");
    delay(1000);
  }


    frames.reserve(MAX_FRAMES);
  //oledWord("setting cap thershold to 5,1");
  
  // Step 1: Stop electrode operation
  cap.writeRegister(0x5E, 0x00); // ECR = 0x00 (stop mode)

  // Step 2: Set touch/release thresholds (ELE0–ELE2 example)
  cap.writeRegister(0x41, 0x03); // ELE0 Touch
  cap.writeRegister(0x4B, 0x01); // ELE0 Release
  cap.writeRegister(0x42, 0x03); // ELE1 Touch
  cap.writeRegister(0x4C, 0x01); // ELE1 Release
  cap.writeRegister(0x43, 0x01); // ELE2 Touch
  cap.writeRegister(0x4D, 0x00); // ELE2 Release
  
  // Step 3: Set filter & debounce settings (optional, but good idea)
  cap.writeRegister(0x2B, 0x01); // MHD rising
  cap.writeRegister(0x2C, 0x01); // NHD amount
  // 0x00 before
  cap.writeRegister(0x2D, 0x10); // NCL rising
  cap.writeRegister(0x2E, 0x00); // FDL rising

  cap.writeRegister(0x2F, 0x01); // MHD falling
  cap.writeRegister(0x30, 0x01); // NHD falling
  // 0xFF before
  cap.writeRegister(0x31, 0x64); // NCL falling
  cap.writeRegister(0x32, 0x30); // FDL falling
  
  cap.writeRegister(0x5B, 0x01); // Debounce: 1 touch, 1 release

  // Step 4: Configure baseline tracking control
  cap.writeRegister(0x7B, 0x00); // Baseline filter config (optional for tuning)
  //cap.writeRegister(0x7E, 90);  // Target level
  //cap.writeRegister(0x7C, 117); // USL = 90 * 1.3
  //cap.writeRegister(0x7D, 63);  // LSL = 90 * 0.7
  // Step 5: Enable 3 electrodes with baseline tracking
  cap.writeRegister(0x5E, 0b10000000 + 12); // ECR: 0b10000011 (baseline tracking + 3 electrodes)
      delay(250);

  // Set "random" seed
  //randomSeed(analogRead(BAT_SENS));
  useRealRandomGenerator(true);
  
  // Initialize system time to compile time if unset
  time_t now = time(nullptr);
  if (now < 1577836800) { // before 2020 -> unset
    const char* months = "JanFebMarAprMayJunJulAugSepOctNovDec";
    char mstr[4]; mstr[3] = 0;
    int month = 1, day=1, year=2025, hour=0, min=0, sec=0;
    // __DATE__="Mmm dd yyyy"  __TIME__="hh:mm:ss"
    sscanf(__TIME__, "%d:%d:%d", &hour, &min, &sec);
    sscanf(__DATE__, "%3s %d %d", mstr, &day, &year);
    const char* p = strstr(months, mstr);
    if (p) month = ((int)(p - months) / 3) + 1;
    struct tm t = {};
    t.tm_year = year - 1900;
    t.tm_mon  = month - 1;
    t.tm_mday = day;
    t.tm_hour = hour;
    t.tm_min  = min;
    t.tm_sec  = sec;
    time_t build = mktime(&t);
    if (build != (time_t)-1) {
      struct timeval tv = { build, 0 };
      settimeofday(&tv, nullptr);
    }
  }
  // end setupSys()
  // Restore last app on wake if configured (e.g., wake into CALC)
  {
    Preferences prefs;
    prefs.begin("PocketMage", true);
    bool restore = prefs.getBool("RESTORE_ON_WAKE", RESTORE_ON_WAKE);
    int lastApp = prefs.getInt("LastAppState", HOME);
    String lastFile = prefs.getString("editingFile", "");
    prefs.end();

    if (restore) {
      switch (static_cast<AppState>(lastApp)) {
        case HOME:
          CurrentAppState = HOME; CurrentKBState = NORMAL; newState = true; break;
        case TXT:
          if (lastFile.length() > 0) editingFile = lastFile; TXT_INIT(); break;
        case FILEWIZ:
          FILEWIZ_INIT(); break;
        case SETTINGS:
          SETTINGS_INIT(); break;
        case TASKS:
          TASKS_INIT(); break;
        case CALENDAR:
          CALENDAR_INIT(); break;
        case JOURNAL:
          JOURNAL_INIT(); break;
        case LEXICON:
          LEXICON_INIT(); break;
        case CALC:
          CALC_INIT(); break;
        default: break;
      }
    }
  }
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
