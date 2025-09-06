//  .o88o.              .o.                      
//  888 `"             .888.                     
// o888oo  .ooooo.    .8"888.  .ooooo.  oooo d8b 
//  888   d88' `88b  .8' `888. d88' `88b `888"8P 
//  888   888ooo888 .88ooo8888.888ooo888  888    
//  888   888    .o .8'     `88888    .o  888    
// o888o  `Y8bod8P'o88o     o8888`Y8bod8P'o888o   

#include <pocketmage.h>
#include <time.h>

// Minimal calendar app adapted for SPIFFS
// Events file format: /sys/events.txt with lines "YYYYMMDD|Title"

static String currentLineCal = "";
static int calYear = 0;
static int calMonth = 0; // 1..12

static String two(int v){ if (v<10) return "0"+String(v); return String(v); }
static String ymd(int y,int m,int d){ return String(y)+two(m)+two(d); }

static bool hasEventOn(const String& yyyymmdd) {
  String all = SD().readFileToString(SPIFFS, "/sys/events.txt");
  int p = 0;
  while (p < (int)all.length()) {
    int nl = all.indexOf('\n', p);
    if (nl < 0) nl = all.length();
    String line = all.substring(p, nl);
    if (line.startsWith(yyyymmdd + "|")) return true;
    p = nl + 1;
  }
  return false;
}

static void addEvent(const String& yyyymmdd, const String& title) {
  String all = SD().readFileToString(SPIFFS, "/sys/events.txt");
  all += yyyymmdd + "|" + title + "\n";
  SD().writeFile(SPIFFS, "/sys/events.txt", all.c_str());
}

static void delEvent(const String& yyyymmdd, const String& title) {
  String all = SD().readFileToString(SPIFFS, "/sys/events.txt");
  String out;
  int p=0; while (p < (int)all.length()){
    int nl = all.indexOf('\n', p); if (nl<0) nl = all.length();
    String line = all.substring(p, nl);
    if (!line.startsWith(yyyymmdd+"|") || (title.length()>0 && line != (yyyymmdd+"|"+title))) {
      if (line.length()>0) out += line + "\n";
    }
    p = nl+1;
  }
  SD().writeFile(SPIFFS, "/sys/events.txt", out.c_str());
}

static int daysInMonth(int y,int m){
  static const int d[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
  int r = d[m-1];
  bool leap = ((y%4==0 && y%100!=0) || (y%400==0));
  if (m==2 && leap) r=29; return r;
}

void CALENDAR_INIT() {
  CurrentAppState = CALENDAR;
  CurrentKBState  = NORMAL;
  newState = true;
  currentLineCal = "";
  time_t now = time(nullptr);
  struct tm t; localtime_r(&now,&t);
  calYear = t.tm_year + 1900; calMonth = t.tm_mon + 1;
}

void processKB_CALENDAR(){
  int currentMillis = millis();
  if (currentMillis - KBBounceMillis < KB_COOLDOWN) return;
  char inchar = KB().updateKeypress();
  if (inchar == 0) return;
  if (inchar == 13) { // Enter
    String cmd = currentLineCal; currentLineCal = "";
    cmd.trim(); cmd.toLowerCase();
    if (cmd.startsWith("addevent ")) {
      String rest = cmd.substring(9); // YYYYMMDD Title
      int sp = rest.indexOf(' ');
      if (sp>0) { addEvent(rest.substring(0,sp), rest.substring(sp+1)); OLED().oledWord("Added"); }
    } else if (cmd.startsWith("delevent ")) {
      String rest = cmd.substring(9);
      int sp = rest.indexOf(' ');
      if (sp>0) delEvent(rest.substring(0,sp), rest.substring(sp+1));
      else delEvent(rest, "");
      OLED().oledWord("Deleted");
    } else if (cmd == "next") {
      calMonth++; if (calMonth>12){ calMonth=1; calYear++; }
    } else if (cmd == "prev") {
      calMonth--; if (calMonth<1){ calMonth=12; calYear--; }
    } else if (cmd == "home") {
      CurrentAppState = HOME; CurrentHOMEState = HOME_HOME; newState=true; CurrentKBState = NORMAL; return;
    }
    newState = true;
  } else if (inchar == 8) { // backspace
    if (currentLineCal.length()>0) currentLineCal.remove(currentLineCal.length()-1);
  } else if (inchar == 20) { // clear
    currentLineCal = "";
  } else if (inchar == 12) { // home
    CurrentAppState = HOME; CurrentHOMEState = HOME_HOME; newState=true; CurrentKBState=NORMAL; return;
  } else {
    currentLineCal += inchar;
  }

  // OLED input echo
  if (currentMillis - OLEDFPSMillis >= (1000/OLED_MAX_FPS)) {
    OLEDFPSMillis = currentMillis;
    OLED().oledLine(currentLineCal, false);
  }
}

void einkHandler_CALENDAR(){
  if (!newState) return; newState=false;
  display.fillScreen(GxEPD_WHITE);
  EINK().setTXTFont(&FreeSerif9pt7b);
  String title = String("Calendar  ") + String(calYear) + "-" + two(calMonth);
  display.setCursor(8, 20); display.print(title);

  // draw simple month listing
  int dy = 16; int y = 40; int dmax = daysInMonth(calYear, calMonth);
  for (int d=1; d<=dmax; ++d){
    String tag = ymd(calYear,calMonth,d);
    String line = two(calMonth) + "/" + two(d) + ": ";
    if (hasEventOn(tag)) line += "*";
    display.setCursor(8, y); display.print(line);
    y += dy; if (y > 210) break; // stop if out of screen
  }
  EINK().drawStatusBar("addevent YYYYMMDD title | prev/next");
  EINK().multiPassRefresh(1);
}
