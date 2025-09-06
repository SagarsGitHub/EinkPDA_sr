//  ooooooooooooo ooooooo  ooooo ooooooooooooo  //
//  8'   888   `8  `8888    d8'  8'   888   `8  //
//       888         Y888..8P         888       //
//       888          `8888'          888       //
//       888         .8PY888.         888       //
//       888        d8'  `888b        888       //
//      o888o     o888o  o88888o     o888o      //

#include <pocketmage.h>

static String currentLine = "";
static bool updateScreen = false;

// Font includes
// Mono
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoOblique9pt7b.h>
#include <Fonts/FreeMonoBoldOblique9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeMonoBoldOblique18pt7b.h>
#include <Fonts/FreeMonoBoldOblique24pt7b.h>

// Serif
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/FreeSerifBold9pt7b.h>
#include <Fonts/FreeSerifItalic9pt7b.h>
#include <Fonts/FreeSerifBoldItalic9pt7b.h>
#include <Fonts/FreeSerifBold12pt7b.h>
#include <Fonts/FreeSerifBold18pt7b.h>
#include <Fonts/FreeSerifBold24pt7b.h>
#include <Fonts/FreeSerifBoldItalic12pt7b.h>
#include <Fonts/FreeSerifBoldItalic18pt7b.h>
#include <Fonts/FreeSerifBoldItalic24pt7b.h>

// Sans
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansOblique9pt7b.h>
#include <Fonts/FreeSansBoldOblique9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSansBoldOblique12pt7b.h>
#include <Fonts/FreeSansBoldOblique18pt7b.h>
#include <Fonts/FreeSansBoldOblique24pt7b.h>

// ------------------ Fonts ------------------
#define SPECIAL_PADDING 20      // Padding for lists, code blocks, quote blocks
#define SPACEWIDTH_SYMBOL "n"   // n is roughly the width of a space
#define HEADING_LINE_PADDING 8  // Padding between each line
#define NORMAL_LINE_PADDING  2   

enum FontFamily { serif = 0, sans = 1, mono = 2 };
uint8_t fontStyle = sans;

struct FontMap {
    const GFXfont* normal;
    const GFXfont* normal_B;
    const GFXfont* normal_I;
    const GFXfont* normal_BI;

    const GFXfont* h1;
    const GFXfont* h1_B;
    const GFXfont* h1_I;
    const GFXfont* h1_BI;

    const GFXfont* h2;
    const GFXfont* h2_B;
    const GFXfont* h2_I;
    const GFXfont* h2_BI;

    const GFXfont* h3;
    const GFXfont* h3_B;
    const GFXfont* h3_I;
    const GFXfont* h3_BI;

    const GFXfont* code;
    const GFXfont* code_B;
    const GFXfont* code_I;
    const GFXfont* code_BI;

    const GFXfont* quote;
    const GFXfont* quote_B;
    const GFXfont* quote_I;
    const GFXfont* quote_BI;

    const GFXfont* list;
    const GFXfont* list_B;
    const GFXfont* list_I;
    const GFXfont* list_BI;
};

FontMap fonts[3];

void initFonts() {
  // Mono
  fonts[mono].normal   = &FreeMono9pt7b;
  fonts[mono].normal_B = &FreeMonoBold9pt7b;
  fonts[mono].normal_I = &FreeMonoOblique9pt7b;
  fonts[mono].normal_BI= &FreeMonoBoldOblique9pt7b;

  fonts[mono].h1       = &FreeMonoBold24pt7b;
  fonts[mono].h1_B     = &FreeMonoBold24pt7b; // Already bold
  fonts[mono].h1_I     = &FreeMonoBoldOblique24pt7b;
  fonts[mono].h1_BI    = &FreeMonoBoldOblique24pt7b;

  fonts[mono].h2       = &FreeMonoBold18pt7b;
  fonts[mono].h2_B     = &FreeMonoBold18pt7b;
  fonts[mono].h2_I     = &FreeMonoBoldOblique18pt7b;
  fonts[mono].h2_BI    = &FreeMonoBoldOblique18pt7b;

  fonts[mono].h3       = &FreeMonoBold12pt7b;
  fonts[mono].h3_B     = &FreeMonoBold12pt7b;
  fonts[mono].h3_I     = &FreeMonoBoldOblique12pt7b;
  fonts[mono].h3_BI    = &FreeMonoBoldOblique12pt7b;

  fonts[mono].code     = &FreeMono9pt7b;
  fonts[mono].code_B   = &FreeMono9pt7b;
  fonts[mono].code_I   = &FreeMono9pt7b;
  fonts[mono].code_BI  = &FreeMono9pt7b;

  fonts[mono].quote    = &FreeMono9pt7b;
  fonts[mono].quote_B  = &FreeMonoBold9pt7b;
  fonts[mono].quote_I  = &FreeMonoOblique9pt7b;
  fonts[mono].quote_BI = &FreeMonoBoldOblique9pt7b;

  fonts[mono].list     = &FreeMono9pt7b;
  fonts[mono].list_B   = &FreeMonoBold9pt7b;
  fonts[mono].list_I   = &FreeMonoOblique9pt7b;
  fonts[mono].list_BI  = &FreeMonoBoldOblique9pt7b;

  // Serif
  fonts[serif].normal   = &FreeSerif9pt7b;
  fonts[serif].normal_B = &FreeSerifBold9pt7b;
  fonts[serif].normal_I = &FreeSerifItalic9pt7b;
  fonts[serif].normal_BI= &FreeSerifBoldItalic9pt7b;

  fonts[serif].h1       = &FreeSerifBold24pt7b;
  fonts[serif].h1_B     = &FreeSerifBold24pt7b;
  fonts[serif].h1_I     = &FreeSerifBoldItalic24pt7b;
  fonts[serif].h1_BI    = &FreeSerifBoldItalic24pt7b;

  fonts[serif].h2       = &FreeSerifBold18pt7b;
  fonts[serif].h2_B     = &FreeSerifBold18pt7b;
  fonts[serif].h2_I     = &FreeSerifBoldItalic18pt7b;
  fonts[serif].h2_BI    = &FreeSerifBoldItalic18pt7b;

  fonts[serif].h3       = &FreeSerifBold12pt7b;
  fonts[serif].h3_B     = &FreeSerifBold12pt7b;
  fonts[serif].h3_I     = &FreeSerifBoldItalic12pt7b;
  fonts[serif].h3_BI    = &FreeSerifBoldItalic12pt7b;

  fonts[serif].code     = &FreeMono9pt7b;
  fonts[serif].code_B   = &FreeMono9pt7b;
  fonts[serif].code_I   = &FreeMono9pt7b;
  fonts[serif].code_BI  = &FreeMono9pt7b;

  fonts[serif].quote    = &FreeSerif9pt7b;
  fonts[serif].quote_B  = &FreeSerifBold9pt7b;
  fonts[serif].quote_I  = &FreeSerifItalic9pt7b;
  fonts[serif].quote_BI = &FreeSerifBoldItalic9pt7b;

  fonts[serif].list     = &FreeSerif9pt7b;
  fonts[serif].list_B   = &FreeSerifBold9pt7b;
  fonts[serif].list_I   = &FreeSerifItalic9pt7b;
  fonts[serif].list_BI  = &FreeSerifBoldItalic9pt7b;

  // Sans
  fonts[sans].normal   = &FreeSans9pt7b;
  fonts[sans].normal_B = &FreeSansBold9pt7b;
  fonts[sans].normal_I = &FreeSansOblique9pt7b;
  fonts[sans].normal_BI= &FreeSansBoldOblique9pt7b;

  fonts[sans].h1       = &FreeSansBold24pt7b;
  fonts[sans].h1_B     = &FreeSansBold24pt7b;
  fonts[sans].h1_I     = &FreeSansBoldOblique24pt7b;
  fonts[sans].h1_BI    = &FreeSansBoldOblique24pt7b;

  fonts[sans].h2       = &FreeSansBold18pt7b;
  fonts[sans].h2_B     = &FreeSansBold18pt7b;
  fonts[sans].h2_I     = &FreeSansBoldOblique18pt7b;
  fonts[sans].h2_BI    = &FreeSansBoldOblique18pt7b;

  fonts[sans].h3       = &FreeSansBold12pt7b;
  fonts[sans].h3_B     = &FreeSansBold12pt7b;
  fonts[sans].h3_I     = &FreeSansBoldOblique12pt7b;
  fonts[sans].h3_BI    = &FreeSansBoldOblique12pt7b;

  fonts[sans].code     = &FreeMono9pt7b;
  fonts[sans].code_B   = &FreeMono9pt7b;
  fonts[sans].code_I   = &FreeMono9pt7b;
  fonts[sans].code_BI  = &FreeMono9pt7b;

  fonts[sans].quote    = &FreeSans9pt7b;
  fonts[sans].quote_B  = &FreeSansBold9pt7b;
  fonts[sans].quote_I  = &FreeSansOblique9pt7b;
  fonts[sans].quote_BI = &FreeSansBoldOblique9pt7b;

  fonts[sans].list     = &FreeSans9pt7b;
  fonts[sans].list_B   = &FreeSansBold9pt7b;
  fonts[sans].list_I   = &FreeSansOblique9pt7b;
  fonts[sans].list_BI  = &FreeSansBoldOblique9pt7b;
}

void setFontStyle(FontFamily f) { fontStyle = f; }

const GFXfont* pickFont(char style, bool bold, bool italic) {
  FontMap &fm = fonts[fontStyle];  // currently active family

  switch (style) {
    case '1': // H1
      if (bold && italic) return fm.h1_BI;
      if (bold) return fm.h1_B;
      if (italic) return fm.h1_I;
      return fm.h1;

    case '2': // H2
      if (bold && italic) return fm.h2_BI;
      if (bold) return fm.h2_B;
      if (italic) return fm.h2_I;
      return fm.h2;

    case '3': // H3
      if (bold && italic) return fm.h3_BI;
      if (bold) return fm.h3_B;
      if (italic) return fm.h3_I;
      return fm.h3;

    case '>': // Quote
      if (bold && italic) return fm.quote_BI;
      if (bold) return fm.quote_B;
      if (italic) return fm.quote_I;
      return fm.quote;

    case '-': // List
      if (bold && italic) return fm.list_BI;
      if (bold) return fm.list_B;
      if (italic) return fm.list_I;
      return fm.list;

    case 'C': // Code
      if (bold && italic) return fm.code_BI;
      if (bold) return fm.code_B;
      if (italic) return fm.code_I;
      return fm.code;

    default: // Normal
      if (bold && italic) return fm.normal_BI;
      if (bold) return fm.normal_B;
      if (italic) return fm.normal_I;
      return fm.normal;
  }
}

// ------------------ Document ------------------
ulong indexCounter = 0;
ulong lineScroll = 0;

struct wordObject {
  String text;
  bool bold;
  bool italic;
};

struct LineObject {
  ulong index;
  std::vector<wordObject> words;
};

struct DocLine {
  char style;                                        // Markdown style: '1', '2', '3', '>', '-', etc.
  String line;                                       // Raw line content
  std::vector<wordObject> words;                     // Parsed words with formatting
  std::vector<LineObject> lines;                     // split into line objects
  
  // Parse the line into wordObjects
  void parseWords() {
    words.clear();
    int i = 0;
    while (i < line.length()) {
      if (line[i] == '*' && i+1 < line.length() && line[i+1] == '*') {
        // Bold **...**
        int end = line.indexOf("**", i + 2);
        if (end == -1) end = line.length();
        String segment = line.substring(i + 2, end);
        splitIntoWords(segment, true, false);
        i = end + 2;
      } 
      else if (line[i] == '*') {
        // Italic *...*
        int end = line.indexOf("*", i + 1);
        if (end == -1) end = line.length();
        String segment = line.substring(i + 1, end);
        splitIntoWords(segment, false, true);
        i = end + 1;
      } 
      else {
        // Normal text until next * or **
        int nextBold = line.indexOf("**", i);
        int nextItalic = line.indexOf("*", i);
        int end = line.length();
        if (nextBold >= 0) end = min(end, nextBold);
        if (nextItalic >= 0) end = min(end, nextItalic);
        String segment = line.substring(i, end);
        splitIntoWords(segment, false, false);
        i = end;
      }
    }
  }

  // Split word objects into lines
  void splitToLines() {
    uint16_t textWidth = EINK().getDisplay().width();

    if (style == '>' || style == '-' || style == 'L' || style == 'C') {
      textWidth -= SPECIAL_PADDING;
    }

    lines.clear();
    LineObject currentLine;
    int lineWidth = 0;

    for (auto &w : words) {
      const GFXfont *font = pickFont(style, w.bold, w.italic);
      EINK().setTXTFont(font);

      int16_t x1, y1;
      uint16_t wpx, hpx;
      EINK().getDisplay().getTextBounds(w.text.c_str(), 0, 0, &x1, &y1, &wpx, &hpx);

      int spaceWidth;
      EINK().getDisplay().getTextBounds(SPACEWIDTH_SYMBOL, 0, 0, &x1, &y1, (uint16_t*)&spaceWidth, &hpx);

      // Calculate width for this word plus space
      int addWidth = wpx + spaceWidth + 12; // IDK why 12 makes the text wrap work perfectly...

      // If the word doesn't fit, start a new line
      if (lineWidth > 0 && (lineWidth + addWidth > textWidth)) {
        currentLine.index = indexCounter++;
        lines.push_back(currentLine);

        currentLine.words.clear();
        lineWidth = 0;
      }

      currentLine.words.push_back(w);
      lineWidth += addWidth;
    }

    if (!currentLine.words.empty()) {
      currentLine.index = indexCounter++;
      lines.push_back(currentLine);
    }
  }
  
  // Display this DocLine
  int displayLine(int startX, int startY) {
    int cursorY = startY;

    // Horizontal Rules just print a line
    if (style == 'H') {
      EINK().getDisplay().drawFastHLine(0, cursorY + 3, EINK().getDisplay().width(), GxEPD_BLACK);
      EINK().getDisplay().drawFastHLine(0, cursorY + 4, EINK().getDisplay().width(), GxEPD_BLACK);
      return 8;
    }

    // Lists and Blockquotes are padded on the left
    if (style =='>' || style == '-' || style == 'L') startX += SPECIAL_PADDING;
    else if (style == 'C') startX += (SPECIAL_PADDING/2);

    for (auto &ln : lines) {
      if (ln.index < lineScroll) continue; // skip lines above scroll

      int cursorX = startX;

      // 1. Find max height for this line
      uint16_t max_hpx = 0;
      for (auto &w : ln.words) {
        const GFXfont *font = pickFont(style, w.bold, w.italic);
        EINK().setTXTFont(font);
        int16_t x1, y1;
        uint16_t wpx, hpx;
        EINK().getDisplay().getTextBounds(w.text.c_str(), cursorX, cursorY, &x1, &y1, &wpx, &hpx);
        if (hpx > max_hpx) max_hpx = hpx;
      }

      // Add space for headings
      if (style =='1' || style == '2' || style == '3') max_hpx += 4;

      // 2. Draw all words at the same baseline
      for (auto &w : ln.words) {
        const GFXfont *font = pickFont(style, w.bold, w.italic);
        EINK().setTXTFont(font);

        int16_t x1, y1;
        uint16_t wpx, hpx;
        EINK().getDisplay().getTextBounds(w.text.c_str(), cursorX, cursorY, &x1, &y1, &wpx, &hpx);

        // Draw word at the baseline
        EINK().getDisplay().setCursor(cursorX, cursorY + max_hpx);
        EINK().getDisplay().print(w.text);

        // Advance cursor (word width + space)
        int16_t sx1, sy1;
        uint16_t sw, sh;
        EINK().getDisplay().getTextBounds(SPACEWIDTH_SYMBOL, cursorX, cursorY, &sx1, &sy1, &sw, &sh);

        cursorX += wpx + sw;
      }

      // Move down for next line
      uint8_t padding = 0;
      if (style =='1' || style == '2' || style == '3') padding = HEADING_LINE_PADDING;
      else padding = NORMAL_LINE_PADDING;
      cursorY += max_hpx + padding;
    }

    // Blockquotes get a vertical line on the left
    if (style =='>') {
      EINK().getDisplay().drawFastVLine(SPECIAL_PADDING/2, startY, (cursorY-startY), GxEPD_BLACK);
      EINK().getDisplay().drawFastVLine((SPECIAL_PADDING/2)+1, startY, (cursorY-startY), GxEPD_BLACK);
    }

    // Code Blocks get a vertical line on each side
    else if (style == 'C') {
      EINK().getDisplay().drawFastVLine(SPECIAL_PADDING/4, startY, (cursorY-startY), GxEPD_BLACK);
      EINK().getDisplay().drawFastVLine(EINK().getDisplay().width() - (SPECIAL_PADDING/4), startY, (cursorY-startY), GxEPD_BLACK);
      EINK().getDisplay().drawFastVLine((SPECIAL_PADDING/4) + 1, startY, (cursorY-startY), GxEPD_BLACK);
      EINK().getDisplay().drawFastVLine(EINK().getDisplay().width() - (SPECIAL_PADDING/4) - 1, startY, (cursorY-startY), GxEPD_BLACK);
    }

    // Headings get a horizontal line below them
    else if (style =='1' || style == '2' || style == '3') {
      EINK().getDisplay().drawFastHLine(0, cursorY - 2, EINK().getDisplay().width(), GxEPD_BLACK);
      EINK().getDisplay().drawFastHLine(0, cursorY - 3, EINK().getDisplay().width(), GxEPD_BLACK);
    }

    return cursorY - startY;
  }

private:
  // Helper: split a string segment into words and push them into words vector
  void splitIntoWords(const String &segment, bool bold, bool italic) {
    int start = 0;
    while (start < segment.length()) {
      int nextSpace = segment.indexOf(' ', start);
      if (nextSpace == -1) nextSpace = segment.length();
      String word = segment.substring(start, nextSpace);
      if (word.length() > 0) {
        words.push_back({word, bold, italic});
      }
      start = nextSpace + 1;
    }
  }
};

void populateLines(std::vector<DocLine> &docLines) {
  indexCounter = 0;

  for (auto &doc : docLines) {
    doc.parseWords();
    doc.splitToLines();
  }
}

std::vector<DocLine> docLines;

// Load File
void loadMarkdownFile(const String &path) {
  setCpuFrequencyMhz(240);
  delay(50);

  docLines.clear();
  File file = SPIFFS.open(path.c_str());
  if (!file) {
    Serial.println("Failed to open file");
    return;
  }

  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim();
    char style = 'T';
    String content = line; // default is full line

    if (line.startsWith("### ")) {
      style = '3';
      content = line.substring(4); // remove "### "
    }
    else if (line.startsWith("## ")) {
      style = '2';
      content = line.substring(3); // remove "## "
    }
    else if (line.startsWith("# ")) {
      style = '1';
      content = line.substring(2); // remove "# "
    }
    else if (line.startsWith("> ")) {
      style = '>';
      content = line.substring(2); // remove "> "
    }
    else if (line.startsWith("- ")) {
      style = '-';
      //content = line.substring(2); // remove "- "
    }
    else if (line == "---") {
      style = 'H';
      content = ""; // horizontal line has no content
    }
    else if ((line.startsWith("'''")) || (line.startsWith("'") && line.endsWith("'"))) {
      if (line.startsWith("'''")) {
        content = line.substring(3);
      }
      else {
        content = line.substring(1, line.length() - 1);
      }

      style = 'C';
    }
    else if (line.length() > 2 && isDigit(line.charAt(0)) && line.charAt(1) == '.' && line.charAt(2) == ' ') {
      style = 'L';
      //content = line.substring(3); // remove "1. ", "2. ", etc.
    }

    docLines.push_back({style, content, {}});
  }

  file.close();

  // Populate all the lines
  populateLines(docLines);

  if (SAVE_POWER) setCpuFrequencyMhz(80);
}

// Count number of display lines
int getTotalDisplayLines() {
  int total = 0;
  for (const auto& doc : docLines) {
      total += doc.lines.size();
  }
  return total;
}

// Display the entire document
int displayDocument(int startX = 0, int startY = 0) {
  int cursorY = startY;

  for (auto &doc : docLines) {
    // Display this DocLine, offset by current cursorY
    int heightUsed = doc.displayLine(startX, cursorY);

    // If the line is off the bottom of the screen, stop drawing
    if (cursorY > EINK().getDisplay().height()) break;

    cursorY += heightUsed;
  }

  // Return total height used
  return cursorY - startY;
}

// Scroll
void updateScroll() {
  static int lastTouchPos = -1;
  static unsigned long lastTouchTime = 0;
  static int prev_lineScroll = 0;

  uint16_t touched = cap.touched();  // Read touch state
  int touchPos = -1;

  // Find the first active touch point (lowest index first)
  for (int i = 0; i < 9; i++) {
    if (touched & (1 << i)) {
      touchPos = i;
      Serial.print("Prev pad: ");
      Serial.print(lastTouchPos);
      Serial.print("   Touched pad: ");
      Serial.println(touchPos);
      break;
    }
  }

  unsigned long currentTime = millis();

  if (touchPos != -1) {  // If a touch is detected
    Serial.println("Touch Detected");
    if (lastTouchPos != -1) {  // Compare with previous touch
      int touchDelta = abs(touchPos - lastTouchPos);
      if (touchDelta <= 2) {  // Ignore large jumps (adjust threshold if needed)
        int maxScroll = getTotalDisplayLines();
        // REVERSED SCROLL DIRECTION:
        if (touchPos < lastTouchPos && lineScroll < maxScroll) {
          prev_lineScroll = lineScroll;
          lineScroll++;
        } else if (touchPos > lastTouchPos && lineScroll > 0) {
          prev_lineScroll = lineScroll;
          lineScroll--;
        }
      }
    }
    lastTouchPos = touchPos;  // Always update lastTouchPos
    lastTouchTime = currentTime;  // Reset timeout timer
  } 
  else if (lastTouchPos != -1 && (currentTime - lastTouchTime > TOUCH_TIMEOUT_MS)) {
    // RESET LASTTOUCHPOS AFTER TIMEOUT
    lastTouchPos = -1;
    // ONLY UPDATE IF SCROLL HAS CHANGED
    if (prev_lineScroll != lineScroll) updateScreen = true;
  }
}

void TXT_INIT() {
  //loadMarkdownFile("/markdownTest.txt");
  //OLED().oledWord("FILE LOADED");
  delay(500);

  initFonts();
  setFontStyle(serif);

  CurrentAppState = TXT;
  lineScroll = 0;
  updateScreen = true;
}

void einkHandler_TXT_NEW() {
  if (updateScreen) {
    OLED().oledWord("DISPLAYING TXT");
    delay(200);
    updateScreen = false;
    EINK().getDisplay().firstPage();
    do {
    EINK().getDisplay().setFullWindow();
    EINK().getDisplay().fillScreen(GxEPD_WHITE);
    displayDocument();
    } while (EINK().getDisplay().nextPage());
    //EINK().refresh();
  }
}

void processKB_TXT_NEW() {
  int currentMillis = millis();
  //Make sure oled only updates at 10FPS
  if (currentMillis - OLEDFPSMillis >= (1000/10 /*OLED_MAX_FPS*/)) {
    OLEDFPSMillis = currentMillis;
    OLED().oledLine(currentLine, false);
  }
  
  if (currentMillis - KBBounceMillis >= KB_COOLDOWN) {  
    // update scroll
    updateScroll();
    currentLine = String(lineScroll) + "/" + String(getTotalDisplayLines());

    char inchar = KB().updateKeypress();
    // HANDLE INPUTS
    //No char recieved
    if (inchar == 0);   
    // Home recieved
    else if (inchar == 12 || inchar == 8) {
      CurrentAppState = HOME; CurrentHOMEState = HOME_HOME;
      currentLine     = "";
      newState        = true;
      CurrentKBState  = NORMAL;
    }
  }
}
