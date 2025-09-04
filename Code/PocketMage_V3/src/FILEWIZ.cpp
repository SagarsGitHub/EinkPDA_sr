//  oooooooooooo ooooo ooooo        oooooooooooo oooooo   oooooo     oooo ooooo  oooooooooooo  //
//  `888'     `8 `888' `888'        `888'     `8  `888.    `888.     .8'  `888' d'""""""d888'  //
//   888          888   888          888           `888.   .8888.   .8'    888        .888P    //
//   888oooo8     888   888          888oooo8       `888  .8'`888. .8'     888       d888'     //
//   888    "     888   888          888    "        `888.8'  `888.8'      888     .888P       //
//   888          888   888       o  888       o      `888'    `888'       888    d888'    .P  //
//  o888o        o888o o888ooooood8 o888ooooood8       `8'      `8'       o888o .8888888888P   //
#include <pocketmage.h>


enum FileWizState { WIZ0_, WIZ1_, WIZ1_YN, WIZ2_R, WIZ2_C, WIZ3_ };
FileWizState CurrentFileWizState = WIZ0_;

String currentWord = "";
static String currentLine = "";

void FILEWIZ_INIT() {
  CurrentAppState = FILEWIZ;
  KB().setState(FUNC);
  EINK().forceSlowFullUpdate(true);
  newState = true;
}

void processKB_FILEWIZ() {
  if (OLEDPowerSave) {
    u8g2.setPowerSave(0);
    OLEDPowerSave = false;
  }
  int currentMillis = millis();

  switch (CurrentFileWizState) {
    case WIZ0_:
      disableTimeout = false;

      KB().setState(FUNC);
      currentMillis = millis();
      //Make sure oled only updates at 60fps
      if (currentMillis - KBBounceMillis >= KB_COOLDOWN) {  
        char inchar = KB().updateKeypress();
        //No char recieved
        if (inchar == 0);
        //BKSP Recieved
        else if (inchar == 127 || inchar == 8 || inchar == 12) {
          HOME_INIT();
          break;
        }
        else if (inchar >= '0' && inchar <= '9') {
          int fileIndex = (inchar == '0') ? 10 : (inchar - '0');
          // SET WORKING FILE
          String selectedFile = filesList[fileIndex - 1];
          if (selectedFile != "-" && selectedFile != "") {
            workingFile = selectedFile;
            // GO TO WIZ1_
            CurrentFileWizState = WIZ1_;
            newState = true;
          }
        }

        currentMillis = millis();
        //Make sure oled only updates at 60fps
        if (currentMillis - OLEDFPSMillis >= (1000/OLED_MAX_FPS)) {
          OLEDFPSMillis = currentMillis;
          OLED().oledLine(currentWord, false);
        }
        KBBounceMillis = currentMillis;
      }
      break;

    case WIZ1_:
      disableTimeout = false;

      KB().setState(FUNC);
      currentMillis = millis();
      //Make sure oled only updates at 60fps
      if (currentMillis - KBBounceMillis >= KB_COOLDOWN) {  
        char inchar = KB().updateKeypress();
        //No char recieved
        if (inchar == 0);
        //BKSP Recieved
        else if (inchar == 127 || inchar == 8 || inchar == 12) {
          CurrentFileWizState = WIZ0_;
          newState = true;
          break;
        }
        else if (inchar >= '1' && inchar <= '4') {
          int fileIndex = (inchar == '0') ? 10 : (inchar - '0');
          // SELECT OPTION
          switch (fileIndex) {
            case 1: // RENAME
              CurrentFileWizState = WIZ2_R;
              newState = true;
              break;
            case 2: //DELETE
              CurrentFileWizState = WIZ1_YN;
              newState = true;
              break;
            case 3: // COPY
              CurrentFileWizState = WIZ2_C;
              newState = true;
              break;
            case 4: // ELABORATE
              break;
          }
        }

        currentMillis = millis();
        //Make sure oled only updates at 60fps
        if (currentMillis - OLEDFPSMillis >= (1000/OLED_MAX_FPS)) {
          OLEDFPSMillis = currentMillis;
          OLED().oledLine(currentWord, false);
        }
        KBBounceMillis = currentMillis;
      }
      break;
    case WIZ1_YN:
      disableTimeout = false;

      KB().setState(NORMAL);
      currentMillis = millis();
      //Make sure oled only updates at 60fps
      if (currentMillis - KBBounceMillis >= KB_COOLDOWN) {  
        char inchar = KB().updateKeypress();
        //No char recieved
        if (inchar == 0);
        //BKSP Recieved
        else if (inchar == 127 || inchar == 8 || inchar == 12) {
          CurrentFileWizState = WIZ1_;
          newState = true;
          break;
        }
        // Y RECIEVED
        else if (inchar == 'y' || inchar == 'Y') {
          // DELETE FILE
          pocketmage().delFile(workingFile);
          
          // RETURN TO FILE WIZ HOME
          CurrentFileWizState = WIZ0_;
          newState = true;
          break;
        }
        // N RECIEVED
        else if (inchar == 'n' || inchar == 'N') {
          // GO BACK
          CurrentFileWizState = WIZ1_;
          newState = true;
          break;
        }

        currentMillis = millis();
        //Make sure oled only updates at 60fps
        if (currentMillis - OLEDFPSMillis >= (1000/OLED_MAX_FPS)) {
          OLEDFPSMillis = currentMillis;
          OLED().oledLine(currentWord, false);
        }
        KBBounceMillis = currentMillis;
      }
      break;
    case WIZ2_R:
      disableTimeout = false;

      //KB().setState(NORMAL);
      currentMillis = millis();
      //Make sure oled only updates at 60fps
      if (currentMillis - KBBounceMillis >= KB_COOLDOWN) {  
        char inchar = KB().updateKeypress();
        //No char recieved
        if (inchar == 0);                                         
        //SHIFT Recieved
        else if (inchar == 17) {                                  
          if (KB().state() == SHIFT) KB().setState(NORMAL);
          else KB().setState(SHIFT);
        }
        //FN Recieved
        else if (inchar == 18) {                                  
          if (KB().state() == FUNC) KB().setState(NORMAL);
          else KB().setState(FUNC);
        }
        //Space Recieved
        else if (inchar == 32) {}
        //ESC / CLEAR Recieved
        else if (inchar == 20) {                                  
          currentWord = "";
        }
        //BKSP Recieved
        else if (inchar == 8) {                  
          if (currentWord.length() > 0) {
            currentWord.remove(currentWord.length() - 1);
          }
        }
        else if (inchar == 12) {
          CurrentFileWizState = WIZ1_;
          KB().setState(NORMAL);
          currentWord = "";
          currentLine = "";
          newState = true;
          break;
        }
        //ENTER Recieved
        else if (inchar == 13) {      
          // RENAME FILE                    
          String newName = "/" + currentWord + ".txt";
          pocketmage().renFile(workingFile, newName);

          // RETURN TO WIZ0
          CurrentFileWizState = WIZ0_;
          KB().setState(NORMAL);
          newState = true;
          currentWord = "";
          currentLine = "";
        }
        //All other chars
        else {
          //Only allow char to be added if it's an allowed char
          if (isalnum(inchar) || inchar == '_' || inchar == '-' || inchar == '.') currentWord += inchar;
          if (inchar >= 48 && inchar <= 57) {}  //Only leave FN on if typing numbers
          else if (KB().state() != NORMAL){
            KB().setState(NORMAL);
          }
        }

        currentMillis = millis();
        //Make sure oled only updates at 60fps
        if (currentMillis - OLEDFPSMillis >= (1000/OLED_MAX_FPS)) {
          OLEDFPSMillis = currentMillis;
          OLED().oledLine(currentWord, false);
        }
      }
      break;
    case WIZ2_C:
      disableTimeout = false;

      //KB().setState(NORMAL);
      currentMillis = millis();
      //Make sure oled only updates at 60fps
      if (currentMillis - KBBounceMillis >= KB_COOLDOWN) {  
        char inchar = KB().updateKeypress();
        //No char recieved
        if (inchar == 0);                                         
        //SHIFT Recieved
        else if (inchar == 17) {                                  
          if (KB().state() == SHIFT) KB().setState(NORMAL);
          else KB().setState(SHIFT);
        }
        //FN Recieved
        else if (inchar == 18) {                                  
          if (KB().state() == FUNC) KB().setState(NORMAL);
          else KB().setState(FUNC);
        }
        //Space Recieved
        else if (inchar == 32) {}
        //ESC / CLEAR Recieved
        else if (inchar == 20) {                                  
          currentWord = "";
        }
        //BKSP Recieved
        else if (inchar == 8) {                  
          if (currentWord.length() > 0) {
            currentWord.remove(currentWord.length() - 1);
          }
        }
        else if (inchar == 12) {
          CurrentFileWizState = WIZ1_;
          KB().setState(NORMAL);
          currentWord = "";
          currentLine = "";
          newState = true;
          break;
        }
        //ENTER Recieved
        else if (inchar == 13) {      
          // RENAME FILE                    
          String newName = "/" + currentWord + ".txt";
          pocketmage().copyFile(workingFile, newName);

          // RETURN TO WIZ0
          CurrentFileWizState = WIZ0_;
          KB().setState(NORMAL);
          newState = true;
          currentWord = "";
          currentLine = "";
        }
        //All other chars
        else {
          //Only allow char to be added if it's an allowed char
          if (isalnum(inchar) || inchar == '_' || inchar == '-' || inchar == '.') currentWord += inchar;
          if (inchar >= 48 && inchar <= 57) {}  //Only leave FN on if typing numbers
          else if (KB().state() != NORMAL){
            KB().setState(NORMAL);
          }
        }

        currentMillis = millis();
        //Make sure oled only updates at 60fps
        if (currentMillis - OLEDFPSMillis >= (1000/OLED_MAX_FPS)) {
          OLEDFPSMillis = currentMillis;
          OLED().oledLine(currentWord, false);
        }
      }
      break;
  
  }
}

void einkHandler_FILEWIZ() {
  switch (CurrentFileWizState) {
    case WIZ0_:
      if (newState) {
        newState = false;
        EINK().resetScreen();

        // DRAW APP
        EINK().drawStatusBar("Select a File (0-9)");
        display.drawBitmap(0, 0, fileWizardallArray[0], 320, 218, GxEPD_BLACK);

        // DRAW FILE LIST
        keypad.disableInterrupts();
        SD().listDir("/");
        keypad.enableInterrupts();

        for (int i = 0; i < MAX_FILES; i++) {
          display.setCursor(30, 54+(17*i));
          display.print(filesList[i]);
        }

        EINK().refresh();
      }
      break;
    case WIZ1_:
      if (newState) {
        newState = false;
        EINK().resetScreen();

        // DRAW APP
        EINK().drawStatusBar("- " + workingFile);
        display.drawBitmap(0, 0, fileWizardallArray[1], 320, 218, GxEPD_BLACK);

        EINK().refresh();
      }
      break;
    case WIZ1_YN:
      if (newState) {
        newState = false;
        EINK().resetScreen();

        // DRAW APP
        EINK().drawStatusBar("DEL:" + workingFile + "?(Y/N)");
        display.drawBitmap(0, 0, fileWizardallArray[1], 320, 218, GxEPD_BLACK);

        EINK().refresh();
      }
      break;
    case WIZ2_R:
      if (newState) {
        newState = false;
        EINK().resetScreen();

        // DRAW APP
        EINK().drawStatusBar("Enter New Filename:");
        display.drawBitmap(0, 0, fileWizardallArray[2], 320, 218, GxEPD_BLACK);

        EINK().refresh();
      }
      break;
    case WIZ2_C:
      if (newState) {
        newState = false;
        EINK().resetScreen();

        // DRAW APP
        EINK().drawStatusBar("Enter Name For Copy:");
        display.drawBitmap(0, 0, fileWizardallArray[2], 320, 218, GxEPD_BLACK);

        EINK().refresh();
      }
      break;
  }
}
