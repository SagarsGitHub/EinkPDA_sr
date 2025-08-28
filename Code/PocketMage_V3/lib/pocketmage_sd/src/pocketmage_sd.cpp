// .d88888b  888888ba  //
// 88.    "' 88    `8b //
// `Y88888b. 88     88 //
//       `8b 88     88 //
// d8'   .8P 88    .8P //
//  Y88888P  8888888P  //

#include <pocketmage_sd.h>
#include <pocketmage_oled.h> 
#include <pocketmage_eink.h> 
#include <config.h> // for FULL_REFRESH_AFTER

// ===================== main functions =====================
// Low-Level SDMMC Operations switch to using internal fs::FS*
void PocketmageSD::listDir(fs::FS &fs, const char *dirname) {
  setCpuFrequencyMhz(240);
  delay(50);
  if (noTimeout_) *noTimeout_ = true;
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    if (noTimeout_) *noTimeout_ = false;
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    if (noTimeout_) *noTimeout_ = false;
    Serial.println(" - not a directory");
    return;
  }

  // Reset fileIndex and initialize filesList with "-"
  fileIndex_ = 0; // Reset fileIndex
  for (int i = 0; i < MAX_FILES; i++) {
    filesList_[i] = "-";
  }

  File file = root.openNextFile();
  while (file && fileIndex_ < MAX_FILES) {
    if (!file.isDirectory()) {
      String fileName = String(file.name());
      
      // Check if file is in the exclusion list
      bool excluded = false;
      for (const String &excludedFile : excludedFiles_) {
        if (fileName.equals(excludedFile) || ("/"+fileName).equals(excludedFile)) {
          excluded = true;
          break;
        }
      }

      if (!excluded) {
        filesList_[fileIndex_++] = fileName; // Store file name if not excluded
      }
    }
    file = root.openNextFile();
  }

  for (int i = 0; i < fileIndex_; i++) { // Only print valid entries
    Serial.println(filesList_[i]);
  }

  if (noTimeout_) *noTimeout_ = false;
  //if (SAVE_POWER) setCpuFrequencyMhz(40);

}
void PocketmageSD::readFile(fs::FS &fs, const char *path) {
  setCpuFrequencyMhz(240);
  delay(50);
  if (noTimeout_) *noTimeout_ = true;
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    if (noTimeout_) *noTimeout_ = false;
    Serial.println("- failed to open file for reading");
    return;
  }

  Serial.println("- read from file:");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
  if (noTimeout_) *noTimeout_ = false;
  //if (SAVE_POWER) setCpuFrequencyMhz(40);
}
String PocketmageSD::readFileToString(fs::FS &fs, const char *path) {
  setCpuFrequencyMhz(240);
  delay(50);

  if (noTimeout_) *noTimeout_ = true;
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    if (noTimeout_) *noTimeout_ = false;
    Serial.println("- failed to open file for reading");
    if (oled_) oled_->oledWord("Load Failed");
    delay(500);
    return "";  // Return an empty string on failure
  }

  Serial.println("- reading from file:");
  String content = "";  // Initialize an empty String to hold the content

  while (file.available()) {
    content += (char)file.read();  // Read each character and append to the String
  }

  file.close();
  if (eink_) eink_->setFullRefreshAfter(FULL_REFRESH_AFTER); //Force a full refresh
  if (noTimeout_) *noTimeout_ = false;
  return content;  // Return the complete String
}
void PocketmageSD::writeFile(fs::FS &fs, const char *path, const char *message) {
  setCpuFrequencyMhz(240);
  delay(50);
  if (noTimeout_) *noTimeout_ = true;
  Serial.printf("Writing file: %s\r\n", path);
  delay(200);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    if (noTimeout_) *noTimeout_ = false;
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("- file written");
  } 
  else {
    Serial.println("- write failed");
  }
  file.close();
  if (noTimeout_) *noTimeout_ = false;
  //if (SAVE_POWER) setCpuFrequencyMhz(40);
}
void PocketmageSD::appendFile(fs::FS &fs, const char *path, const char *message) {
  setCpuFrequencyMhz(240);
  delay(50);
  if (noTimeout_) *noTimeout_ = true;
  Serial.printf("Appending to file: %s\r\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    if (noTimeout_) *noTimeout_ = false;
    Serial.println("- failed to open file for appending");
    return;
  }
  if (file.println(message)) {
    Serial.println("- message appended");
  } 
  else {
    Serial.println("- append failed");
  }
  file.close();
  if (noTimeout_) *noTimeout_ = false;
  //if (SAVE_POWER) setCpuFrequencyMhz(40);
}
void PocketmageSD::renameFile(fs::FS &fs, const char *path1, const char *path2) {
  setCpuFrequencyMhz(240);
  delay(50);
  if (noTimeout_) *noTimeout_ = true;
  Serial.printf("Renaming file %s to %s\r\n", path1, path2);
  if (fs.rename(path1, path2)) {
    Serial.println("- file renamed");
  } 
  else {
    Serial.println("- rename failed");
  }
  if (noTimeout_) *noTimeout_ = false;
  //if (SAVE_POWER) setCpuFrequencyMhz(40);
}

void PocketmageSD::deleteFile(fs::FS &fs, const char *path) {
  setCpuFrequencyMhz(240);
  delay(50);
  if (noTimeout_) *noTimeout_ = true;
  Serial.printf("Deleting file: %s\r\n", path);
  if (fs.remove(path)) {
    Serial.println("- file deleted");
  } 
  else {
    Serial.println("- delete failed");
  }
  if (noTimeout_) *noTimeout_ = false;
  //if (SAVE_POWER) setCpuFrequencyMhz(40);
}