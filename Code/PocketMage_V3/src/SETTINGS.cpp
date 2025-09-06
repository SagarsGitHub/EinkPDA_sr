// SETTINGS app (SPIFFS device, no external RTC)
#include <pocketmage.h>
#include <Preferences.h>

enum SettingsState { settings0 };
static SettingsState CurrentSettingsState = settings0;
static String currentLine = "";

static int stringToIntLocal(String str) {
  str.trim();
  if (str.length() == 0) return -1;
  for (int i=0;i<str.length();++i) if (!isDigit(str[i])) return -1;
  return str.toInt();
}

void SETTINGS_INIT() {
  currentLine = "";
  CurrentAppState = SETTINGS;
  CurrentSettingsState = settings0;
  CurrentKBState  = NORMAL;
  newState = true;
}

static void savePrefsKV(const char* key, int v)      { Preferences p; p.begin("PocketMage", false); p.putInt(key, v);      p.end(); }
static void savePrefsKV(const char* key, bool v)     { Preferences p; p.begin("PocketMage", false); p.putBool(key, v);     p.end(); }
static void savePrefsKV(const char* key, const String& v) { Preferences p; p.begin("PocketMage", false); p.putString(key, v); p.end(); }

static void settingCommandSelect(String command) {
  command.toLowerCase();

  if (command.startsWith("timeset ")) {
    String timePart = command.substring(8);
    setTimeFromString(timePart);
    OLED().oledWord("Time Updated");
    delay(200);
    return;
  }
  if (command.startsWith("lumina ")) {
    String luminaPart = command.substring(7);
    int lumina = stringToIntLocal(luminaPart);
    if (lumina < 0) { OLED().oledWord("Invalid"); delay(500); return; }
    if (lumina > 255) lumina = 255;
    OLED_BRIGHTNESS = lumina; u8g2.setContrast(OLED_BRIGHTNESS);
    savePrefsKV("OLED_BRIGHTNESS", OLED_BRIGHTNESS);
    newState = true; OLED().oledWord("Settings Updated"); delay(200); return;
  }
  if (command.startsWith("timeout ")) {
    String v = command.substring(8);
    int t = stringToIntLocal(v);
    if (t < 0) { OLED().oledWord("Invalid"); delay(500); return; }
    TIMEOUT = t; savePrefsKV("TIMEOUT", TIMEOUT);
    newState = true; OLED().oledWord("Settings Updated"); delay(200); return;
  }
  if (command == "systemclock on"  || command == "clock on")  { SYSTEM_CLOCK = true;  savePrefsKV("SYSTEM_CLOCK", SYSTEM_CLOCK); newState=true; OLED().oledWord("Clock On"); delay(200); return; }
  if (command == "systemclock off" || command == "clock off") { SYSTEM_CLOCK = false; savePrefsKV("SYSTEM_CLOCK", SYSTEM_CLOCK); newState=true; OLED().oledWord("Clock Off"); delay(200); return; }
  if (command == "restore on")  { RESTORE_ON_WAKE = true;  savePrefsKV("RESTORE_ON_WAKE", RESTORE_ON_WAKE); newState=true; OLED().oledWord("Restore On"); delay(200); return; }
  if (command == "restore off") { RESTORE_ON_WAKE = false; savePrefsKV("RESTORE_ON_WAKE", RESTORE_ON_WAKE); newState=true; OLED().oledWord("Restore Off"); delay(200); return; }
  if (command == "nowlater on")  { NOWLATER_ON_SLEEP = true;  savePrefsKV("NOWLATER_ON_SLEEP", NOWLATER_ON_SLEEP); newState=true; OLED().oledWord("NowLater On"); delay(200); return; }
  if (command == "nowlater off") { NOWLATER_ON_SLEEP = false; savePrefsKV("NOWLATER_ON_SLEEP", NOWLATER_ON_SLEEP); newState=true; OLED().oledWord("NowLater Off"); delay(200); return; }
  if (command == "showyear on")  { SHOW_YEAR = true;  savePrefsKV("SHOW_YEAR", SHOW_YEAR); newState=true; OLED().oledWord("ShowYear On"); delay(200); return; }
  if (command == "showyear off") { SHOW_YEAR = false; savePrefsKV("SHOW_YEAR", SHOW_YEAR); newState=true; OLED().oledWord("ShowYear Off"); delay(200); return; }
  if (command == "savepower on")  { SAVE_POWER = true;  savePrefsKV("SAVE_POWER", SAVE_POWER); setCpuFrequencyMhz(POWER_SAVE_FREQ); newState=true; OLED().oledWord("SavePower On"); delay(200); return; }
  if (command == "savepower off") { SAVE_POWER = false; savePrefsKV("SAVE_POWER", SAVE_POWER); setCpuFrequencyMhz(240);          newState=true; OLED().oledWord("SavePower Off"); delay(200); return; }
  if (command == "debug on")  { DEBUG_VERBOSE = true;  savePrefsKV("DEBUG_VERBOSE", DEBUG_VERBOSE); newState=true; OLED().oledWord("Debug On"); delay(200); return; }
  if (command == "debug off") { DEBUG_VERBOSE = false; savePrefsKV("DEBUG_VERBOSE", DEBUG_VERBOSE); newState=true; OLED().oledWord("Debug Off"); delay(200); return; }
  if (command == "homeonboot on")  { HOME_ON_BOOT = true;  savePrefsKV("HOME_ON_BOOT", HOME_ON_BOOT); newState=true; OLED().oledWord("HomeOnBoot On"); delay(200); return; }
  if (command == "homeonboot off") { HOME_ON_BOOT = false; savePrefsKV("HOME_ON_BOOT", HOME_ON_BOOT); newState=true; OLED().oledWord("HomeOnBoot Off"); delay(200); return; }
  if (command.startsWith("fps ")) { int v = stringToIntLocal(command.substring(4)); if (v<1) v=1; if (v>200) v=200; OLED_MAX_FPS=v; savePrefsKV("OLED_MAX_FPS", OLED_MAX_FPS); newState=true; OLED().oledWord("FPS Updated"); delay(200); return; }

  OLED().oledWord("Huh?"); delay(600);
}

void processKB_settings() {
  int currentMillis = millis();
  switch (CurrentSettingsState) {
    case settings0:
      if (currentMillis - KBBounceMillis >= KB_COOLDOWN) {
        char inchar = KB().updateKeypress();
        if (inchar == 0) {}
        else if (inchar == 13) { settingCommandSelect(currentLine); currentLine=""; }
        else if (inchar == 17) { if (CurrentKBState == SHIFT) CurrentKBState = NORMAL; else CurrentKBState = SHIFT; }
        else if (inchar == 18) { if (CurrentKBState == FUNC)  CurrentKBState = NORMAL; else CurrentKBState = FUNC; }
        else if (inchar == 32) { currentLine += " "; }
        else if (inchar == 20) { currentLine = ""; }
        else if (inchar == 8)  { if (currentLine.length() > 0) currentLine.remove(currentLine.length()-1); }
        else if (inchar == 12) { CurrentAppState = HOME; CurrentHOMEState = HOME_HOME; currentLine=""; newState=true; CurrentKBState=NORMAL; }
        else { currentLine += inchar; if (!(inchar >= '0' && inchar <= '9') && CurrentKBState != NORMAL) CurrentKBState = NORMAL; }

        currentMillis = millis();
        if (currentMillis - OLEDFPSMillis >= (1000/OLED_MAX_FPS)) { OLEDFPSMillis = currentMillis; OLED().oledLine(currentLine, false); }
      }
      break;
  }
}

void einkHandler_settings() {
  if (newState) {
    newState = false;
    display.fillScreen(GxEPD_WHITE);
    display.drawBitmap(0, 0, _settings, 320, 218, GxEPD_BLACK);

    display.setFont(&FreeSerif9pt7b);
    // First column
    display.setCursor(8, 42);  display.print(String(OLED_BRIGHTNESS).c_str());
    display.setCursor(8, 65);  display.print(String(TIMEOUT).c_str());
    if (SYSTEM_CLOCK) display.drawBitmap(8, 75, _toggleON, 26, 11, GxEPD_BLACK); else display.drawBitmap(8, 75, _toggleOFF, 26, 11, GxEPD_BLACK);
    if (SHOW_YEAR)   display.drawBitmap(8, 98, _toggleON, 26, 11, GxEPD_BLACK); else display.drawBitmap(8, 98, _toggleOFF, 26, 11, GxEPD_BLACK);
    if (SAVE_POWER)  display.drawBitmap(8,121, _toggleON, 26, 11, GxEPD_BLACK); else display.drawBitmap(8,121, _toggleOFF, 26, 11, GxEPD_BLACK);
    if (DEBUG_VERBOSE) display.drawBitmap(8,144, _toggleON, 26, 11, GxEPD_BLACK); else display.drawBitmap(8,144, _toggleOFF, 26, 11, GxEPD_BLACK);
    if (HOME_ON_BOOT) display.drawBitmap(8,167, _toggleON, 26, 11, GxEPD_BLACK); else display.drawBitmap(8,167, _toggleOFF, 26, 11, GxEPD_BLACK);

    // Second column
    display.setCursor(163, 42); display.print(String(OLED_MAX_FPS).c_str());

    EINK().drawStatusBar("Type a Command:");
    EINK().multiPassRefresh(2);
  }
}
