//  ooooo   ooooo   .oooooo.   ooo        ooooo oooooooooooo  //
//  `888'   `888'  d8P'  `Y8b  `88.       .888' `888'     `8  //
//   888     888  888      888  888b     d'888   888          //
//   888ooooo888  888      888  8 Y88. .P  888   888oooo8     //
//   888     888  888      888  8  `888'   888   888    "     //
//   888     888  `88b    d88'  8    Y     888   888       o  //
//  o888o   o888o  `Y8bood8P'  o8o        o888o o888ooooood8  //
#include <pocketmage.h>
#include <time.h>

static String currentLine = "";

void HOME_INIT() {
  CurrentAppState = HOME;
  currentLine     = "";
  CurrentKBState  = NORMAL;
  CurrentHOMEState = HOME_HOME;
  newState = true;
}

void commandSelect(String command) {
  command.toLowerCase();

  // OPEN IN FILE WIZARD
  if (command.startsWith("-")) {
    command = removeChar(command, ' ');
    command = removeChar(command, '-');
    keypad.disableInterrupts();
    SD().listDir(SPIFFS, "/");
    keypad.enableInterrupts();

    for (uint8_t i = 0; i < (sizeof(filesList) / sizeof(filesList[0])); i++) {
      String lowerFileName = filesList[i]; 
      lowerFileName.toLowerCase();
      if (command == lowerFileName || (command+".txt") == lowerFileName || ("/"+command+".txt") == lowerFileName) {
        workingFile = filesList[i];
        FILEWIZ_INIT();
        return;
      }
    }
  }

  // OPEN IN TXT EDITOR
  if (command.startsWith("/")) {
    command = removeChar(command, ' ');
    command = removeChar(command, '/');
    keypad.disableInterrupts();
    SD().listDir(SPIFFS, "/");
    keypad.enableInterrupts();

    for (uint8_t i = 0; i < (sizeof(filesList) / sizeof(filesList[0])); i++) {
      String lowerFileName = filesList[i]; 
      lowerFileName.toLowerCase();
      if (command == lowerFileName || (command+".txt") == lowerFileName || ("/"+command+".txt") == lowerFileName) {
        editingFile = filesList[i];
        TXT_INIT();
        return;
      }
    }
  }

  // Dice Roll
  if (command.startsWith("roll d")) {
    String numStr = command.substring(6);
    int sides = numStr.toInt();
    if (sides < 1) {
      OLED().oledWord("Please enter a valid number");
      delay(2000);
    } 
    else if (sides == 1) {
      OLED().oledWord("D1: you rolled a 1, duh!");
      delay(2000);
    }
    else {
      int roll = (esp_random() % sides) + 1;
      if (roll == sides)  OLED().oledWord("D" + String(sides) + ": " + String(roll) + "!!!");
      else if (roll == 1) OLED().oledWord("D" + String(sides) + ": " + String(roll) + " :(");
      else                OLED().oledWord("D" + String(sides) + ": " + String(roll));
      delay(3000);
      CurrentKBState = NORMAL;
    }
  }

  else if (command == "home") {
    OLED().oledWord("You're home, silly!");
    delay(1000);
  } 
  /////////////////////////////
  else if (command == "note" || command == "text" || command == "write" || command == "notebook" || command == "notepad" || command == "txt" || command == "1") {
    TXT_INIT();
  }
  /////////////////////////////
  else if (command == "file wizard" || command == "wiz" || command == "file wiz" || command == "file" || command == "2") {
    FILEWIZ_INIT();
  }
  /////////////////////////////
  else if (command == "tasks" || command == "task" || command == "6") {
    TASKS_INIT();
  }
  /////////////////////////////
  else if (command == "bluetooth" || command == "bt" || command == "4") {
    // OPEN BLUETOOTH
  }
  /////////////////////////////
  else if (command == "calc" || command == "calculator" || command == "math" || command == "10") {
    CALC_INIT();
  }
  else if (command == "settings" || command == "set" || command == "5") {
    SETTINGS_INIT();
  }
  else if (command == "lex" || command == "lexicon" || command == "dict" || command == "dictionary" || command == "9") {
    LEXICON_INIT();
  }
  else if (command == "calendar" || command == "cal" || command == "7") {
    CALENDAR_INIT();
  }
  else if (command == "journal" || command == "8") {
    JOURNAL_INIT();
  }
  /////////////////////////////
  else if (command == "i farted") {
    OLED().oledWord("That smells");
    delay(1000);
  } 
  else if (command == "poop") {
    OLED().oledWord("Yuck");
    delay(1000);
  } 
  else if (command == "hello") {
    OLED().oledWord("Hey, you!");
    delay(1000);
  } 
  else if (command == "hi") {
    OLED().oledWord("What's up?");
    delay(1000);
  } 
  else if (command == "i love you") {
    OLED().oledWord("luv u 2 <3");
    delay(1000);
  } 
  else if (command == "what can you do") {
    OLED().oledWord("idk man");
    delay(1000);
  } 
  else if (command == "alexa") {
    OLED().oledWord("...");
    delay(1000);
  } 
}

void drawHome() {
  EINK().getDisplay().setFullWindow();
  EINK().getDisplay().firstPage();
  do {
  EINK().getDisplay().setRotation(3);
  EINK().getDisplay().fillScreen(GxEPD_WHITE);
  
  int16_t x1, y1;
  uint16_t charWidth, charHeight;
  uint8_t appsPerRow = 5; // Number of apps per row
  uint8_t spacingX = 60;  // Horizontal spacing
  uint8_t spacingY = 60;  // Vertical spacing
  uint8_t iconSize = 40;  // Icon width and height
  uint8_t startX = 20;    // Initial X position
  uint8_t startY = 20;    // Initial Y position

  EINK().setTXTFont(&FreeSerif9pt7b);
  for (int i = 0; i < sizeof(appIcons) / sizeof(appIcons[0]); i++) {
    int row = i / appsPerRow;
    int col = i % appsPerRow;
    
    int xPos = startX + (spacingX * col);
    int yPos = startY + (spacingY * row);

    EINK().getDisplay().drawBitmap(xPos, yPos, appIcons[i], iconSize, iconSize, GxEPD_BLACK);
    EINK().getDisplay().getTextBounds(appStateNames[i], 0, 0, &x1, &y1, &charWidth, &charHeight);
    EINK().getDisplay().setCursor(xPos + (iconSize / 2) - (charWidth / 2), yPos + iconSize + 13);
    EINK().getDisplay().print(appStateNames[i]);
  }
  EINK().setTXTFont(&FreeMonoBold9pt7b);

  EINK().drawStatusBar("Type a Command:");
    } while (EINK().getDisplay().nextPage());
}

void drawThickLine(int x0, int y0, int x1, int y1, int thickness) {
  float dx = x1 - x0;
  float dy = y1 - y0;
  float length = sqrt(dx * dx + dy * dy);
  float stepX = dx / length;
  float stepY = dy / length;

  for (float i = 0; i <= length; i += thickness / 2.0) {
    int cx = round(x0 + i * stepX);
    int cy = round(y0 + i * stepY);
    EINK().getDisplay().fillCircle(cx, cy, thickness / 2, GxEPD_BLACK);
  }
}

void processKB_HOME() {
  int currentMillis = millis();

  switch (CurrentHOMEState) {
    case HOME_HOME:
      if (currentMillis - KBBounceMillis >= KB_COOLDOWN) {  
        char inchar = KB().updateKeypress();
        // HANDLE INPUTS
        //No char recieved
        if (inchar == 0);   
        //CR Recieved
        else if (inchar == 13) {                          
          commandSelect(currentLine);
          currentLine = "";
        }                                      
        //SHIFT Recieved
        else if (inchar == 17) {                                  
          if (CurrentKBState == SHIFT) CurrentKBState = NORMAL;
          else CurrentKBState = SHIFT;
        }
        //FN Recieved
        else if (inchar == 18) {                                  
          if (CurrentKBState == FUNC) CurrentKBState = NORMAL;
          else CurrentKBState = FUNC;
        }
        //Space Recieved
        else if (inchar == 32) {                                  
          currentLine += " ";
        }
        // Home recieved
        else if (inchar == 12) {
          CurrentAppState = HOME;
          currentLine     = "";
          newState        = true;
          CurrentKBState  = NORMAL;
        }
        //ESC / CLEAR Recieved
        else if (inchar == 20) {                                  
          currentLine = "";
        }
        //BKSP Recieved
        else if (inchar == 8) {                  
          if (currentLine.length() > 0) {
            currentLine.remove(currentLine.length() - 1);
          }
        }
        else {
          currentLine += inchar;
          if (inchar >= 48 && inchar <= 57) {}  //Only leave FN on if typing numbers
          else if (CurrentKBState != NORMAL) {
            CurrentKBState = NORMAL;
          }
        }

        currentMillis = millis();
        //Make sure oled only updates at OLED_MAX_FPS
        if (currentMillis - OLEDFPSMillis >= (1000/OLED_MAX_FPS)) {
          OLEDFPSMillis = currentMillis;
          OLED().oledLine(currentLine, false);
        }
      }
      break;
  }
}

void einkHandler_HOME() {
  switch (CurrentHOMEState) {
    case HOME_HOME:
      if (newState) {
        newState = false;

        drawHome();
       
        //EINK().multiPassRefesh(2);
      }
      break;
    case NOWLATER: {
      static int prevMinuteNL = -1;
      time_t raw = time(nullptr);
      struct tm t; localtime_r(&raw, &t);
      if (newState || t.tm_min != prevMinuteNL) {
        newState = false;
        prevMinuteNL = t.tm_min;

        auto &d = EINK().getDisplay();
        d.setFullWindow();
        d.firstPage();
        do {
          d.fillScreen(GxEPD_WHITE);
          // Big clock time
          EINK().setTXTFont(&FreeMonoBold24pt7b);
          char tim[6];
          snprintf(tim, sizeof(tim), "%02d:%02d", t.tm_hour, t.tm_min);
          int16_t x1,y1; uint16_t w,h; d.getTextBounds(tim, 0, 0, &x1, &y1, &w, &h);
          d.setCursor((d.width() - w)/2, 90);
          d.print(tim);

          // Date below
          EINK().setTXTFont(&FreeSerif9pt7b);
          char datebuf[16];
          snprintf(datebuf, sizeof(datebuf), "%02d/%02d/%02d", t.tm_mon+1, t.tm_mday, (t.tm_year+1900)%100);
          d.getTextBounds(datebuf, 0, 0, &x1, &y1, &w, &h);
          d.setCursor((d.width() - w)/2, 120);
          d.print(datebuf);

          // Footer hint
          EINK().setTXTFont(&FreeMonoBold9pt7b);
          EINK().drawStatusBar("Press power to wake");
        } while (d.nextPage());
      }
      break;
    }
  }
}
