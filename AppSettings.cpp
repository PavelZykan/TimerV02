#include "AppSettings.h"

BuzzerSettings AppSettings::buzzerSettings;
CalibrationSettings AppSettings::calibrationSettings;  

void AppSettings::setupSettings() {
  EEPROM.get(0, AppSettings::buzzerSettings);  

  if (AppSettings::buzzerSettings.timerDelayMin == 0 || AppSettings::buzzerSettings.timerDelayMin == 65535) {
    AppSettings::buzzerSettings.timerDelayMin = 4;
  }  
  if (AppSettings::buzzerSettings.timerDelayMax == 0 || AppSettings::buzzerSettings.timerDelayMax == 65535) {
    AppSettings::buzzerSettings.timerDelayMax = 6;
  }

  EEPROM.get(sizeof(BuzzerSettings), AppSettings::calibrationSettings);
  
  if (AppSettings::calibrationSettings.xAxisMax == 65535) {
    AppSettings::calibrationSettings.xAxisMax = 0;
  }
  if (AppSettings::calibrationSettings.yAxisMax == 65535) {
    AppSettings::calibrationSettings.yAxisMax = 0;
  }
  if (AppSettings::calibrationSettings.zAxisMax == 65535) {
    AppSettings::calibrationSettings.zAxisMax = 0;
  }
}

void AppSettings::saveSettings() {
  EEPROM.put(0, AppSettings::buzzerSettings);
  EEPROM.put(sizeof(BuzzerSettings), AppSettings::calibrationSettings);
}
