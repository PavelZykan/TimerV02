#include "AppMenu.h"
#include "AppLogic.h"

#include <avr/pgmspace.h>
const char string_0[] PROGMEM = "ArTimer ";   // "String 0" etc are strings to store - change to suit.
const char string_1[] PROGMEM = "Run: UP or DOWN";
const char string_2[] PROGMEM = "Start/Stop mode";
const char string_3[] PROGMEM = "Time: ";
const char string_4[] PROGMEM = "Timer delay";
const char string_5[] PROGMEM = "Min: ";
const char string_6[] PROGMEM = "Max: ";
const char string_7[] PROGMEM = " s";
const char string_8[] PROGMEM = " ";
const char string_9[] PROGMEM = "G readings";
const char string_10[] PROGMEM = "G calibration";

LiquidCrystal_I2C AppMenu::lcd = LiquidCrystal_I2C(0x3F, 16, 2);  // Set the LCD I2C address

LiquidLine AppMenu::welcome_line1 = LiquidLine(0, 0, string_0, swVersion);
LiquidLine AppMenu::welcome_line2 = LiquidLine(0, 1, string_1);

LiquidScreen AppMenu::welcome_screen = LiquidScreen(welcome_line1, welcome_line2);

LiquidLine AppMenu::startStopLine1 = LiquidLine(0, 0, string_2);
LiquidLine AppMenu::startStopLine2 = LiquidLine(0, 1, string_3, AppLogic::elapsedTime, string_7);
LiquidScreen AppMenu::startStopScreen = LiquidScreen(startStopLine1, startStopLine2);

LiquidLine AppMenu::timerDelayMinLine1 = LiquidLine(0, 0, string_4);
LiquidLine AppMenu::timerDelayMinLine2 = LiquidLine(0, 1, string_5, AppSettings::buzzerSettings.timerDelayMin, string_7);
LiquidScreen AppMenu::timerDelayMinScreen = LiquidScreen(timerDelayMinLine1, timerDelayMinLine2);

LiquidLine AppMenu::timerDelayMaxLine1 = LiquidLine(0, 0, string_4);
LiquidLine AppMenu::timerDelayMaxLine2 = LiquidLine(0, 1, string_6, AppSettings::buzzerSettings.timerDelayMax, string_7);
LiquidScreen AppMenu::timerDelayMaxScreen = LiquidScreen(timerDelayMaxLine1, timerDelayMaxLine2);

LiquidLine AppMenu::gCalibrationSetupLine1 = LiquidLine(0, 0, string_10);
LiquidLine AppMenu::gCalibrationSetupLine2 = LiquidLine(0, 1, AppLogic::calibrationMessage);
LiquidScreen AppMenu::gCalibrationSetupScreen = LiquidScreen(gCalibrationSetupLine1, gCalibrationSetupLine2);

LiquidMenu AppMenu::menu = LiquidMenu(lcd);

void AppMenu::update() {
  AppMenu::menu.update();
}

void AppMenu::setupMenu() {  
  AppMenu::lcd.init(); //initialize the lcd
  AppMenu::lcd.backlight(); //open the backlight 
  
  // This methid needs to be called when using an I2C display library.
  AppMenu::menu.init();

  welcome_line1.set_asProgmem(1);
  welcome_line2.set_asProgmem(1);
  startStopLine1.set_asProgmem(1);
  startStopLine2.set_asProgmem(1);
  startStopLine2.set_asProgmem(3);
  timerDelayMinLine1.set_asProgmem(1);
  timerDelayMinLine2.set_asProgmem(1);
  timerDelayMinLine2.set_asProgmem(3);
  timerDelayMaxLine1.set_asProgmem(1);
  timerDelayMaxLine2.set_asProgmem(1);
  timerDelayMaxLine2.set_asProgmem(3);
  gCalibrationSetupLine1.set_asProgmem(1);

  AppMenu::menu.add_screen(welcome_screen);
  AppMenu::menu.add_screen(startStopScreen);
  
  AppMenu::menu.add_screen(timerDelayMinScreen);
  AppMenu::menu.add_screen(timerDelayMaxScreen);
  AppMenu::menu.add_screen(gCalibrationSetupScreen);

  AppMenu::welcome_line1.attach_function(1, AppLogic::startBuzzer);
  AppMenu::welcome_line1.attach_function(2, AppLogic::startCountdown);

  AppMenu::startStopLine1.attach_function(1, AppLogic::startStopModeDown);
  AppMenu::startStopLine1.attach_function(2, AppLogic::startStopModeUp);

  AppMenu::timerDelayMinLine1.attach_function(1, AppLogic::decMinDelay);
  AppMenu::timerDelayMinLine1.attach_function(2, AppLogic::incMinDelay);

  AppMenu::timerDelayMaxLine1.attach_function(1, AppLogic::decMaxDelay);
  AppMenu::timerDelayMaxLine1.attach_function(2, AppLogic::incMaxDelay);

  AppMenu::gCalibrationSetupLine1.attach_function(1, AppLogic::toggleCalibration);
  AppMenu::gCalibrationSetupLine1.attach_function(2, AppLogic::toggleCalibration);

  AppMenu::menu.update();
};

void AppMenu::prevScreen() {
  menu.previous_screen();
}

void AppMenu::nextScreen() {
  menu.next_screen();
}

void AppMenu::callFunction(unsigned short idx) {
  menu.call_function(0, idx);
}

