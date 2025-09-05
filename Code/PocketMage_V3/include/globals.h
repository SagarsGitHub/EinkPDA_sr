#ifndef GLOBALS_H
#define GLOBALS_H


// LIBRARIES
#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <U8g2lib.h>
#include <Adafruit_TCA8418.h>
#include <vector>
#include <Buzzer.h>
#include <USB.h>
#include <USBMSC.h>
#include <SD_MMC.h>
#include <Preferences.h>
#include <Adafruit_MPR121.h>
#include <esp_cpu.h>
#include <RTClib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <assets.h>
#include <config.h>

// ===================== DISPLAY =====================
// E-ink display
extern GxEPD2_BW<GxEPD2_310_GDEQ031T10, GxEPD2_310_GDEQ031T10::HEIGHT> display;
// OLED 
extern U8G2_SSD1326_ER_256X32_F_4W_HW_SPI u8g2;

// ===================== INPUT DEVICES =====================
// Keypad controller
extern Adafruit_TCA8418 keypad; // Matrix keypad

// Touch slider (capacitive)
extern Adafruit_MPR121 cap; // Touch slider
extern volatile long int dynamicScroll;      // Dynamic scroll offset
extern volatile long int prev_dynamicScroll; // Previous scroll offset
extern int lastTouch;                        // Last touch event
extern unsigned long lastTouchTime;          // Last touch time

// ===================== AUDIO =====================
// Buzzer for sound feedback
extern Buzzer buzzer;

// ===================== RTC =====================
// Real-time clock
extern RTC_PCF8563 rtc; // RTC chip
extern const char daysOfTheWeek[7][12]; // Day names

// ===================== USB & STORAGE =====================
// USB mass storage controller
extern USBMSC msc;           // USB MSC object
extern bool mscEnabled;      // Is USB MSC active?
extern sdmmc_card_t* card;   // SD card pointer

// ===================== SYSTEM SETTINGS =====================
// Persistent preferences
extern Preferences prefs;        // NVS preferences
extern int TIMEOUT;              // Auto sleep timeout (seconds)
extern bool DEBUG_VERBOSE;       // Extra debug output
extern bool SYSTEM_CLOCK;        // Show clock on screen
extern bool SHOW_YEAR;           // Show year in clock
extern bool SAVE_POWER;          // Enable power saving mode
extern bool ALLOW_NO_MICROSD;    // Allow running without SD card
extern bool HOME_ON_BOOT;        // Start home app on boot
extern int OLED_BRIGHTNESS;      // OLED brightness (0-255)
extern int OLED_MAX_FPS;         // OLED max FPS

// ===================== SYSTEM STATE =====================
extern int OLEDFPSMillis;            // Last OLED FPS update time
extern int KBBounceMillis;           // Last keyboard debounce time
extern volatile int timeoutMillis;   // Timeout tracking
extern volatile int prevTimeMillis;  // Previous time for timeout
extern volatile bool TCA8418_event;  // Keypad interrupt event
extern volatile bool PWR_BTN_event;  // Power button event
extern volatile bool newState;       // App state changed
extern bool noTimeout;               // Disable timeout
extern volatile bool OLEDPowerSave;  // OLED power save mode
extern volatile bool disableTimeout; // Disable timeout globally
extern volatile int battState;       // Battery state
extern int prevTime;                 // Previous time (minutes)
extern TaskHandle_t einkHandlerTaskHandle; // E-Ink handler task

// ===================== SD STATE =====================
extern volatile bool SDCARD_INSERT;  // SD card inserted event
extern bool noSD;                    // No SD card present
extern volatile bool SDActive;       // SD card active

// ===================== FILES & TEXT =====================
extern String editingFile;           // Currently edited file
extern String workingFile;           // Working file name
extern String filesList[MAX_FILES];  // List of files

// ===================== APP STATES =====================
enum KBState { NORMAL, SHIFT, FUNC };    // Keyboard state
enum AppState { HOME, TXT, FILEWIZ, USB_APP, BT, SETTINGS, TASKS, CALENDAR, JOURNAL, LEXICON };
extern const String appStateNames[];     // App state names
extern const unsigned char *appIcons[12]; // App icons
extern AppState CurrentAppState;         // Current app state

// ===================== TXT APP =====================
extern volatile bool newLineAdded;           // New line added in TXT
extern std::vector<String> allLines;         // All lines in TXT

// ===================== TASKS APP =====================
extern std::vector<std::vector<String>> tasks; // Task list

// ===================== HOME APP =====================
enum HOMEState { HOME_HOME, NOWLATER };       // Home app states
extern HOMEState CurrentHOMEState;            // Current home state

// ===================== FUNCTION PROTOTYPES =====================

// <FILEWIZ.cpp>
void FILEWIZ_INIT();
void processKB_FILEWIZ();
void einkHandler_FILEWIZ();

// <TXT.cpp>
void TXT_INIT();
void processKB_TXT_NEW();
void einkHandler_TXT_NEW();

// <HOME.cpp>
void HOME_INIT();
void einkHandler_HOME();
void processKB_HOME();

// <TASKS.cpp>
void TASKS_INIT();
void sortTasksByDueDate(std::vector<std::vector<String>> &tasks);
void updateTaskArray();
void einkHandler_TASKS();
void processKB_TASKS();

// <settings.cpp>
void SETTINGS_INIT();
void processKB_settings();
void einkHandler_settings();
void settingCommandSelect(String command);

// <USB.cpp>
void USB_INIT();
void processKB_USB();
void einkHandler_USB();

// <CALENDAR.cpp>
void CALENDAR_INIT();
void processKB_CALENDAR();
void einkHandler_CALENDAR();

// <LEXICON.cpp>
void LEXICON_INIT();
void processKB_LEXICON();
void einkHandler_LEXICON();

// <JOURNAL.cpp>
void JOURNAL_INIT();
void processKB_JOURNAL();
void einkHandler_JOURNAL();

// <PocketMage>
void einkHandler(void *parameter);
void applicationEinkHandler();
void processKB();

#endif // GLOBALS_H
