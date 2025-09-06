//   .oooooo.                      .o.                      
//  d8P'  `Y8b                    .888.                     
// 888      888  .oooooooo       .8"888.  .ooooo.  oooo d8b 
// 888      888 888' `88b       .8' `888. d88' `88b `888"8P 
// 888      888 888   888      .88ooo8888.888ooo888  888    
// `88b    d88' `88bod8P'     .8'     `88888    .o  888    
//  `Y8bood8P'   `8oooooo.   o88o     o8888`Y8bod8P'o888o   

#include <pocketmage.h>
#include <time.h>

static String currentLineJournal = "";

static String twoJ(int v){ if (v<10) return "0"+String(v); return String(v); }
static String todayPath(){
  time_t now = time(nullptr); struct tm t; localtime_r(&now,&t);
  String ymd = String(t.tm_year+1900) + twoJ(t.tm_mon+1) + twoJ(t.tm_mday);
  return String("/journal/") + ymd + ".txt";
}

void JOURNAL_INIT(){
  CurrentAppState = JOURNAL; CurrentKBState = NORMAL; newState = true; currentLineJournal = "";
}

void processKB_JOURNAL(){
  int currentMillis = millis();
  if (currentMillis - KBBounceMillis < KB_COOLDOWN) return;
  char inchar = KB().updateKeypress();
  if (inchar == 0) return;
  if (inchar == 13) {
    String cmd = currentLineJournal; currentLineJournal = ""; cmd.trim();
    if (cmd == "open today") {
      editingFile = todayPath(); TXT_INIT(); return;
    } else if (cmd.startsWith("open ")) {
      String ymd = cmd.substring(5); ymd.trim();
      if (ymd.length()==8) { editingFile = String("/journal/") + ymd + ".txt"; TXT_INIT(); return; }
    } else if (cmd == "home") {
      CurrentAppState = HOME; CurrentHOMEState = HOME_HOME; newState=true; CurrentKBState=NORMAL; return;
    }
    OLED().oledWord("Try: open today | open YYYYMMDD");
  } else if (inchar == 8) { if (currentLineJournal.length()>0) currentLineJournal.remove(currentLineJournal.length()-1); }
  else if (inchar == 20) { currentLineJournal = ""; }
  else if (inchar == 12) { CurrentAppState = HOME; CurrentHOMEState = HOME_HOME; newState=true; CurrentKBState=NORMAL; return; }
  else { currentLineJournal += inchar; }

  if (currentMillis - OLEDFPSMillis >= (1000/OLED_MAX_FPS)) { OLEDFPSMillis = currentMillis; OLED().oledLine(currentLineJournal, false); }
}

void einkHandler_JOURNAL(){
  if (!newState) return; newState=false;
  display.fillScreen(GxEPD_WHITE);
  EINK().setTXTFont(&FreeSerif9pt7b);
  display.setCursor(8, 20); display.print("Journal");
  EINK().drawStatusBar("open today | open YYYYMMDD");
  EINK().multiPassRefresh(1);
}
