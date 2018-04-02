#ifndef BUTTONS_H_
#define BUTTONS_H_

#include <Wire.h> 
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidMenu.h>
#include <buttons.h>
#include "consts.h"

class AppButtons {
  public:  
    static Button left;
    static Button right;
    static Button up;
    static Button down;
    
    static void setupButtons();
};

#endif
