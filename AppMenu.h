#ifndef APPMENU_H_
#define APPMENU_H_

#include <Wire.h> 
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidMenu.h>
#include <buttons.h>
#include "consts.h"
#include "AppSettings.h"

class AppMenu {
  public:
    static void setupMenu();

    static void update();

    static void prevScreen();

    static void nextScreen();

    static void callFunction(unsigned short idx);

  private:
    static LiquidCrystal_I2C lcd; 
    
    static LiquidLine welcome_line1;
    static LiquidLine welcome_line2;
    
    static LiquidScreen welcome_screen;
    
    static LiquidLine startStopLine1;
    static LiquidLine startStopLine2;
    static LiquidScreen startStopScreen;
    
    static LiquidLine timerDelayMinLine1;
    static LiquidLine timerDelayMinLine2;
    static LiquidScreen timerDelayMinScreen;
    
    static LiquidLine timerDelayMaxLine1;
    static LiquidLine timerDelayMaxLine2;
    static LiquidScreen timerDelayMaxScreen;
    
    static LiquidLine gCalibrationViewLine1;
    static LiquidLine gCalibrationViewLine2;
    static LiquidScreen gCalibrationViewScreen;
    
    static LiquidLine gCalibrationSetupLine1;
    static LiquidLine gCalibrationSetupLine2;
    static LiquidScreen gCalibrationSetupScreen;
    
    static LiquidMenu menu;
};

#endif
