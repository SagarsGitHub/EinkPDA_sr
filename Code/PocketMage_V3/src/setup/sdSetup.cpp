#include <pocketmage.h>


// Initialization of sd class
static PocketmageSD sd;

// Setup for SD Class
// @ dependencies:
//   - setupOled()
//   - setupEINK()
void setupSD() {
  // SPIFFS SETUP
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    Serial.println("SPIFFS Mount Failed");
    delay(1000);
  }
  wireSD();

  setCpuFrequencyMhz(240);
  // Create folders and files if needed
  // LOAD PREFERENCES

}

  // Wire function  for SD class
  // add any global references here + add set function to class header file
  void wireSD() {
      sd.setFileSys(&SPIFFS);
      sd.setOled(OLED());
      sd.setEink(EINK());
      sd.setEditingFile(&editingFile);
      sd.setFilesList(filesList);
      sd.setNoTimeout(&noTimeout);
  }

// Access for other apps
PocketmageSD& SD() { return sd; }
