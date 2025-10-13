//  888888ba  dP     dP d8888888P d8888888P  88888888b  888888ba  //
//  88    `8b 88     88      .d8'      .d8'  88         88    `8b //
// a88aaaa8P' 88     88    .d8'      .d8'   a88aaaa    a88aaaa8P' //
//  88   `8b. 88     88  .d8'      .d8'      88         88   `8b. //
//  88    .88 Y8.   .8P d8'       d8'        88         88     88 //
//  88888888P `Y88888P' Y8888888P Y8888888P  88888888P  dP     dP //

#include <pocketmage.h>

// Initialization of bz class
static PocketmageBZ pm_bz;

PocketmageBZ::PocketmageBZ()
  : buzzer_(BZ_PIN) {}

bool PocketmageBZ::begin(int channel) {
  channel_ = channel;
  buzzer_.begin(channel_);
  begun_ = true;
  return true;
}

void PocketmageBZ::end() {
  if (!begun_) return;
  buzzer_.end(channel_);
  begun_ = false;
}

// Setup for Buzzer Class
void setupBZ() {
  auto& bz = BZ();
  bz.begin();
  bz.playJingle(Jingles::Startup);
}

// Access for other apps
PocketmageBZ& BZ() { return pm_bz; }

// ===================== main functions =====================
void PocketmageBZ::playJingle(const Jingle& jingle) {
  if (jingle.notes == nullptr || jingle.len == 0) {
    return;  // No valid notes to play
  }

  if (!begun_) {
    begin(channel_);
  }

  for (size_t i = 0; i < jingle.len; ++i) {
    buzzer_.sound(jingle.notes[i].key, jingle.notes[i].duration);
  }

  buzzer_.sound(0, 80);  // End the sound
  buzzer_.end(channel_);        // Stop the buzzer
  begun_ = false;
}