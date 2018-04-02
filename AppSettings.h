#ifndef APPSETTINGS_H_
#define APPSETTINGS_H_

#include <Wire.h> 
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidMenu.h>
#include <buttons.h>
#include "consts.h"

struct BuzzerSettings {
  unsigned short timerDelayMin;
  unsigned short timerDelayMax;
};

struct CalibrationSettings {
  unsigned short xAxisMin;
  unsigned short xAxisMax;
  unsigned short yAxisMin;
  unsigned short yAxisMax;
  unsigned short zAxisMin;
  unsigned short zAxisMax;
};

class AppSettings {
  public:
    static BuzzerSettings buzzerSettings;
    static CalibrationSettings calibrationSettings;  

    static void setupSettings();
    static void saveSettings();
};

#endif
