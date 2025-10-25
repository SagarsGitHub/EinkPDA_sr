#ifndef GLOBALS_H
#define GLOBALS_H

// LIBRARIES
#include <USB.h>
#include <USBMSC.h>
#include <SD_MMC.h>
#include <Preferences.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <pocketmage.h>
#include <assets.h>
#include <deque>
#include <map>
#include <stack>
// ===================== USB & STORAGE =====================
// USB mass storage controller
extern USBMSC msc;           // USB MSC object
extern sdmmc_card_t* card;   // SD card pointer

// ===================== SYSTEM STATE =====================
extern Preferences prefs;        // NVS preferences
extern int OLEDFPSMillis;            // Last OLED FPS update time
extern int KBBounceMillis;           // Last keyboard debounce time
extern volatile bool TCA8418_event;  // Keypad interrupt event
extern volatile bool PWR_BTN_event;  // Power button event
extern volatile bool newState;       // App state changed
extern volatile bool OLEDPowerSave;  // OLED power save mode
extern volatile bool disableTimeout; // Disable timeout globally
extern bool fileLoaded;

// extern volatile int prevBattState;   // Previous battery state
extern unsigned int flashMillis;     // Flash timing
extern int prevTime;                 // Previous time (minutes)
extern uint8_t prevSec;              // Previous seconds
extern TaskHandle_t einkHandlerTaskHandle; // E-Ink handler task

extern String OTA1_APP;
extern String OTA2_APP;
extern String OTA3_APP;
extern String OTA4_APP;

// ===================== KEYBOARD STATE =====================
extern volatile bool SDCARD_INSERT;  // SD card inserted event
enum KBState { NORMAL, SHIFT, FUNC };    // Keyboard state

// ===================== APP STATES =====================
enum AppState { HOME, TXT, FILEWIZ, USB_APP, BT, SETTINGS, TASKS, CALENDAR, JOURNAL, LEXICON, APPLOADER, CALC };
extern const String appStateNames[];     // App state names
extern const unsigned char *appIcons[11]; // App icons
extern AppState CurrentAppState;         // Current app state

// ===================== TASKS APP =====================
extern std::vector<std::vector<String>> tasks; // Task list

// ===================== HOME APP =====================
enum HOMEState { HOME_HOME, NOWLATER };       // Home app states
extern HOMEState CurrentHOMEState;            // Current home state

// <FILEWIZ.cpp>
void FILEWIZ_INIT();
void processKB_FILEWIZ();
void einkHandler_FILEWIZ();
String fileWizardMini(bool allowRecentSelect = false, String rootDir = "/");

// <TXT.cpp>
void TXT_INIT();
void processKB_TXT_NEW();
void einkHandler_TXT_NEW();
void saveMarkdownFile(const String& path);

// <HOME.cpp>
void HOME_INIT();
void einkHandler_HOME();
void processKB_HOME();
void mageIdle(bool internalRefresh = true);
void resetIdle();

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

// <APPLOADER.cpp>
void APPLOADER_INIT();
void processKB_APPLOADER();
void einkHandler_APPLOADER();
void rebootToAppSlot(int otaIndex);
void loadAndDrawAppIcon(int x, int y, int otaIndex, bool showName = true, int maxNameChars = 10);

// <PocketMage>
void einkHandler(void *parameter); // moved from EinkFunc.cpp
void applicationEinkHandler();
void processKB();

// ===================== CALC APP =====================
// max refreshes before a full refresh is forced (change with caution)
#define REFRESH_MAX_CALC 5
#define SCROLL_MAX 8
#define SCROLL_MED 4
#define SCROLL_SML 2
#define FRAME_TOP 32                                  // top for large calc frame
#define FRAME_LEFT 10                                 // left for large calc frame
#define FRAME_RIGHT 10                                // right for large calc frame
#define FRAME_BOTTOM 32                               // bottom for large calc frame
enum CALCState { CALC0, CALC1, CALC2, CALC3, CALC4, CALCFONT };
struct Unit {
    const char* name;  
    const char* symbol;  
    // factor * (value + offset) = convert from in to basis unit
    // (inBase / to.factor) - to.offset  = convert from basis unit to output unit
    double factor;         
    double offset;         
};
struct UnitSet {
  const char* category;
  const Unit* data;
  size_t      size;
};
extern Unit emptyUnit;
extern CALCState CurrentCALCState;
extern int refresh_count;
extern String cleanExpression;
extern String calculatedResult;
extern int calcSwitchedStates;
extern String prevLine;
extern std::map<String, double> variables;
extern const char* operatorsCalc[];
extern const size_t operatorsCalcCount;
struct OpEntry { const char* token; uint8_t prec; bool rightAssoc; };
extern const OpEntry OPS[];
extern const size_t OPS_N;
extern const char* functionsCalc[];
extern const size_t functionsCalcCount;
extern const char* constantsCalc[];
extern const size_t constantsCalcCount;
extern std::vector<String> prevTokens;
extern int trigType;

// ===================== FRAME CLASS =====================
# define MAX_FRAMES 100
# define X_OFFSET 4
#pragma region textSource
// bit flags for alignment or future options
enum LineFlags : uint8_t { LF_NONE=0, LF_RIGHT= 1<<0, LF_CENTER= 1<<1 };
struct LineView {
  const char* ptr;   // points to NUL-terminated string in RAM or PROGMEM
  uint16_t    len;   // byte length (no need to include '\0')
  uint8_t     flags; // LineFlags
};
// read-only interface for any line list (PROGMEM table, arena, etc.)
struct TextSource {
  virtual ~TextSource() {}
  virtual size_t   size() const = 0;
  virtual LineView line(size_t i) const = 0; // never allocates
};
template<size_t MAX_LINES, size_t BUF_BYTES>
struct FixedArenaSource : TextSource {
  char     buf[BUF_BYTES];
  uint16_t off[MAX_LINES];
  uint16_t len_[MAX_LINES];
  uint8_t  flags_[MAX_LINES];
  size_t   nLines = 0;
  size_t   used   = 0;

  size_t size() const override { return nLines; }

  LineView line(size_t i) const override {
    return { buf + off[i], len_[i], flags_[i] };
  }

  void clear() { nLines = 0; used = 0; }

  // Returns false if out of capacity; caller can choose to drop the oldest, etc.
  bool pushLine(const char* s, uint16_t L, uint8_t flags = LF_NONE) {
    if (nLines >= MAX_LINES || used + L + 1 > BUF_BYTES) return false;
    memcpy(buf + used, s, L);
    buf[used + L] = '\0';
    off[nLines]   = (uint16_t)used;
    len_[nLines]  = L;
    flags_[nLines]= flags;
    used         += L + 1;
    nLines++;
    return true;
  }
};
struct ProgmemTableSource : TextSource {
  // table is a PROGMEM array of PROGMEM pointers to '\0'-terminated strings
  const char* const* table; // PROGMEM
  size_t count;

  ProgmemTableSource(const char* const* t, size_t n) : table(t), count(n) {}

  size_t size() const override { return count; }

  LineView line(size_t i) const override {
    const char* p = (const char*)pgm_read_ptr(&table[i]);
    // length from PROGMEM
    uint16_t L = (uint16_t)strlen_P(p);
    return { p, L, LF_NONE };
  }
};

extern const char* const HELP_LINES[] PROGMEM;
extern const size_t HELP_COUNT;
extern const char* const UNIT_TYPES_LINES[] PROGMEM;
extern const size_t UNIT_TYPES_COUNT;
extern const char* const CONV_DIR_LINES[] PROGMEM;
extern const size_t CONV_DIR_COUNT;
extern const char* const CONV_LENGTH_LINES[] PROGMEM;
extern const size_t CONV_LENGTH_COUNT;
extern const char* const CONV_AREA_LINES[] PROGMEM;
extern const size_t CONV_AREA_COUNT;
extern const char* const CONV_VOLUME_LINES[] PROGMEM;
extern const size_t CONV_VOLUME_COUNT;
extern const char* const CONV_MASS_LINES[] PROGMEM;
extern const size_t CONV_MASS_COUNT;
extern const char* const CONV_TEMPERATURE_LINES[] PROGMEM;
extern const size_t CONV_TEMPERATURE_COUNT;
extern const char* const CONV_ENERGY_LINES[] PROGMEM;
extern const size_t CONV_ENERGY_COUNT;
extern const char* const CONV_SPEED_LINES[] PROGMEM;
extern const size_t CONV_SPEED_COUNT;
extern const char* const CONV_PRESSURE_LINES[] PROGMEM;
extern const size_t CONV_PRESSURE_COUNT;
extern const char* const CONV_DATA_LINES[] PROGMEM;
extern const size_t CONV_DATA_COUNT;
extern const char* const CONV_ANGLE_LINES[] PROGMEM;
extern const size_t CONV_ANGLE_COUNT;
extern const char* const CONV_TIME_LINES[] PROGMEM;
extern const size_t CONV_TIME_COUNT;
extern const char* const CONV_POWER_LINES[] PROGMEM;
extern const size_t CONV_POWER_COUNT;
extern const char* const CONV_FORCE_LINES[] PROGMEM;
extern const size_t CONV_FORCE_COUNT;
extern const char* const CONV_FREQUENCY_LINES[] PROGMEM;
extern const size_t CONV_FREQUENCY_COUNT;



extern const UnitSet UnitCatalog[];
extern const size_t  UnitCatalogCount;

extern const ProgmemTableSource* const allUnitLists[];
extern const size_t              AllUnitListsCount;

extern const UnitSet* CurrentUnitSet;
extern const ProgmemTableSource* CurrentUnitListSrc;

extern FixedArenaSource<512, 16384> calcLines;
extern ProgmemTableSource helpSrc;
extern ProgmemTableSource unitTypesSrc;
extern ProgmemTableSource convDirSrc;
extern ProgmemTableSource convLengthSrc;
extern ProgmemTableSource convAreaSrc;
extern ProgmemTableSource convVolumeSrc;
extern ProgmemTableSource convMassSrc;
extern ProgmemTableSource convTemperatureSrc;
extern ProgmemTableSource convEnergySrc;
extern ProgmemTableSource convSpeedSrc;
extern ProgmemTableSource convPressureSrc;
extern ProgmemTableSource convDataSrc;
extern ProgmemTableSource convAngleSrc;
extern ProgmemTableSource convTimeSrc;
extern ProgmemTableSource convPowerSrc;
extern ProgmemTableSource convForceSrc;
extern ProgmemTableSource convFrequencySrc;
extern const UnitSet* CurrentUnitSet; 

#pragma endregion
#pragma region frameSetup
class Frame {
public:
  // what kind of content this frame holds
  enum class Kind : uint8_t { none, text, bitmap };

  // geometry
  int left, right, top, bottom;
  int origLeft, origRight, origTop, origBottom;
  int extendLeft, extendRight, extendTop, extendBottom;
  int bitmapW = 0;
  int bitmapH = 0;
  // flags/state switch to uint8_t flag
  bool cursor   = false;
  bool box      = false;
  bool invert   = false;
  bool overlap  = false;


  int   choice     = -1;
  long  scroll     = 0;
  long  prevScroll = -1;
  int   maxLines   = 0;
  long  lastTotal  = -1;

  // content (only one valid at a time)
  Kind kind = Kind::none;
  const TextSource* source = nullptr;  // for text frames
  const uint8_t* bitmap    = nullptr;  // for bitmap frames
  const GFXfont *font = (GFXfont *)&FreeSerif9pt7b;

  
  const Unit *unit = nullptr;
  // base constructor for common fields
  Frame(int left, int right, int top, int bottom, 
        bool cursor=false, bool box=false)
  : left(left), right(right), top(top), bottom(bottom),
    extendBottom(bottom), origBottom(bottom), cursor(cursor), box(box) {}

  // constructor for text frames
  Frame(int left, int right, int top, int bottom,
        const TextSource* linesPtr,
        bool cur=false, bool bx=false)
  : Frame(left, right, top, bottom, cur, bx) {
    kind   = Kind::text;
    source = linesPtr;
  }

  // constructor for bitmap frames
  Frame(int left, int right, int top, int bottom,
        const uint8_t* bitmapPtr, int width, int height,
        bool cursor=false, bool box=false)
  : Frame(left, right, top, bottom, cursor, box) {
    kind      = Kind::bitmap;
    bitmap    = bitmapPtr;
    bitmapW      = width;
    bitmapH      = height;
  }

  bool hasText()   const { return kind == Kind::text   && source; }
  bool hasBitmap() const { return kind == Kind::bitmap && bitmap; }
};
extern Frame calcScreen;
extern Frame conversionScreen;
extern Frame helpScreen;
extern Frame conversionUnit;
extern Frame conversionDirection;
extern Frame conversionFrameA;
extern Frame conversionFrameB;
extern Frame conversionTypes;
extern Frame testBitmapScreen;
extern Frame testBitmapScreen1;
extern Frame testBitmapScreen2;
extern Frame testTextScreen;
extern Frame *CurrentFrameState;
extern int currentFrameChoice;
extern int frameSelection;
extern std::vector<Frame*> frames;
#pragma endregion

#endif // GLOBALS_H

// <CALC.cpp>

void einkHandler_CALC();
void processKB_CALC();
void CALC_INIT();
  // algorithms NOTE: exposed for other apps if needed
std::deque<String> convertToRPN(String expression);
String evaluateRPN(std::deque<String> rpnQueue,const Unit *convA,const Unit *convB);
std::vector<String> tokenize(const String& expression);


// <FRAMES.cpp>
  // main functions
void einkFramesDynamic(std::vector<Frame*> &frames, bool doFull_);
  // text boxes
std::vector<String> formatText(Frame &frame,int maxTextWidth);
void drawLineInFrame(String &srcLine, int lineIndex, Frame &frame, int usableY, bool clearLine, bool isPartial);
void drawFrameBox(int usableX, int usableY, int usableWidth, int usableHeight,bool invert);
int computeCursorX(Frame &frame, bool rightAlign, bool centerAlign, int16_t x1, uint16_t lineWidth);
  // String formatting
static size_t sliceThatFits(const char* s, size_t n, int maxTextWidth);
std::vector<String> sourceToVector(const TextSource* src);
String frameChoiceString(const Frame& f);  
  //scroll
void updateScroll(Frame *currentFrameState,int prevScroll,int currentScroll, bool reset = false);
void updateScrollFromTouch_Frame();
void oledScrollFrame(); 
//void updateScroll(Frame *currentFrameState,int prevScroll,int currentScroll, bool reset);
void getVisibleRange(Frame *f, long totalLines, long &startLine, long &endLine);
