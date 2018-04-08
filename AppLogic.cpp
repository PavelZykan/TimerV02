#include "AppLogic.h"
#include "AppMenu.h"
#include "AppButtons.h"
#include <Arduino.h>

bool AppLogic::countdownRunning = false;
unsigned long AppLogic::countdownStart = 0L;
unsigned int AppLogic::buzzerDelay;
bool AppLogic::buzzerRunning = false;
bool AppLogic::startStopModeRunning = false;
unsigned long AppLogic::buzzerStarted = 0L;
float AppLogic::elapsedTime = 0.0f;
unsigned long AppLogic::lastElapsedTimeMenuUpdate = 0L;
unsigned short AppLogic::currentGx = 0;
unsigned short AppLogic::currentGy = 0;
unsigned short AppLogic::currentGz = 0;
unsigned long AppLogic::lastGreadingsUpdate = 0L;
char* AppLogic::calibrationMessage;
bool AppLogic::calibrationRunning = false;
unsigned short AppLogic::calibrationVarIdx = 0;
GForceCalibration AppLogic::forceCalibration = GForceCalibration(A1, A2, A3);

void AppLogic::setup() {  
  Serial.begin(9600);
  AppLogic::calibrationMessage = new char[17];
  
  setupCalibrationMessage(false, false);  

  pinMode(buzzerPin, OUTPUT);  
  
  updateCalibrationVal();
  
  AppMenu::setupMenu();
  
  AppButtons::setupButtons();

  AppSettings::setupSettings();

  AppLogic::forceCalibration.setup();
}

void AppLogic::loop() {
  readGforces();
  
  if (AppButtons::left.check()) {
    AppMenu::prevScreen();
  } else if (AppButtons::right.check()) {
    AppMenu::nextScreen();
  } else if (AppButtons::up.check()) {
    AppMenu::callFunction(2);
  } else if (AppButtons::down.check()) {
    AppMenu::callFunction(1);
  } 

  if (countdownRunning) {
    unsigned long timeDiff = millis() - countdownStart;
    if (timeDiff >= buzzerDelay) {
      countdownRunning = false;
      startBuzzer();      
    }
  }
  
  if (buzzerRunning) {
    checkBuzzerStop();
  }
  
  if (startStopModeRunning && !countdownRunning) {
    elapsedTime = (millis() - buzzerStarted) / 1000.0f;
    if (millis() - lastElapsedTimeMenuUpdate > elapsedTimeMenuUpdateDelay){
      AppMenu::update();
      lastElapsedTimeMenuUpdate = millis();
    }
  }
}

void AppLogic::readGforces() {
  currentGx = analogRead(A1);
  currentGy = analogRead(A2);
  currentGz = analogRead(A3);
}

void AppLogic::toggleCalibration() {
  if (!calibrationRunning) {
    calibrationRunning = true;
    setupCalibrationMessage(true, false);
    AppLogic::forceCalibration.takeSample(AppLogic::calibrationVarIdx);
    AppLogic::calibrationVarIdx++;
    setupCalibrationMessage(false, false);
  } else {  
    setupCalibrationMessage(true, false);
    AppLogic::forceCalibration.takeSample(AppLogic::calibrationVarIdx);
    AppLogic::calibrationVarIdx++;    
    if (AppLogic::calibrationVarIdx >= 6) {
      AppLogic::calibrationVarIdx = 0;
      setupCalibrationMessage(false, true);
      AppLogic::forceCalibration.calibrate();
      setupCalibrationMessage(false, false);
      calibrationRunning = false;
    } else {
      setupCalibrationMessage(false, false);
    }
  }
  
  AppMenu::update();
}

void AppLogic::startCountdown() {
  countdownRunning = true;
  countdownStart = millis();
  buzzerDelay = random(AppSettings::buzzerSettings.timerDelayMin * 1000, AppSettings::buzzerSettings.timerDelayMax * 1000);
}

void AppLogic::incMinDelay() {
  if (AppSettings::buzzerSettings.timerDelayMin < 30 && AppSettings::buzzerSettings.timerDelayMin < AppSettings::buzzerSettings.timerDelayMax) {
    AppSettings::buzzerSettings.timerDelayMin++;
    AppSettings::saveSettings();
    AppMenu::update();
  }
}

void AppLogic::decMinDelay() {
  if (AppSettings::buzzerSettings.timerDelayMin > 0) {
    AppSettings::buzzerSettings.timerDelayMin--;
    AppSettings::saveSettings();
    AppMenu::update();
  }
}

void AppLogic::incMaxDelay() {
  if (AppSettings::buzzerSettings.timerDelayMax < 30) {
    AppSettings::buzzerSettings.timerDelayMax++;
    AppSettings::saveSettings();
    AppMenu::update();
  }
}

void AppLogic::decMaxDelay() {
  if (AppSettings::buzzerSettings.timerDelayMax > 0 && AppSettings::buzzerSettings.timerDelayMax > AppSettings::buzzerSettings.timerDelayMin) {
    AppSettings::buzzerSettings.timerDelayMax--;
    AppSettings::saveSettings();
    AppMenu::update();
  }
}

void AppLogic::startStopModeUp() {
  if (!startStopModeRunning) {
    startStopModeRunning = true;
    elapsedTime = 0.0f;
    AppLogic::startCountdown();
  } else {
    startStopModeRunning = false;
    AppMenu::update();
  }
}

void AppLogic::startStopModeDown() {
  if (!startStopModeRunning) {
    elapsedTime = 0.0f;
    startBuzzer();
    AppLogic::startStopModeRunning = true;
  } else {
    startStopModeRunning = false;
    AppMenu::update();
  }
}

void AppLogic::startBuzzer() {
  buzzerRunning = true;
  buzzerStarted = millis();
  tone(buzzerPin, 1000);
}

void AppLogic::checkBuzzerStop() {
  if (millis() - buzzerStarted > buzzerToneLength) {
    AppLogic::stopBuzzer();
  }
}

void AppLogic::stopBuzzer() {
  buzzerRunning = false;
  noTone(buzzerPin);
}

void AppLogic::updateCalibrationVal() {
  if (calibrationVarIdx == 0) {
    //calibrationVarValue = AppSettings::calibrationSettings.xAxisMin;
    //calibrationVarLabel[0] = 'X';calibrationVarLabel[1] = 'M';calibrationVarLabel[2] = 'I';calibrationVarLabel[3] = 'N';calibrationVarLabel[4] = ' ';calibrationVarLabel[5] = 0;
  } else if (calibrationVarIdx == 1) {
    //calibrationVarValue = AppSettings::calibrationSettings.xAxisMax;
    //calibrationVarLabel[0] = 'X';calibrationVarLabel[1] = 'M';calibrationVarLabel[2] = 'A';calibrationVarLabel[3] = 'X';calibrationVarLabel[4] = ' ';calibrationVarLabel[5] = 0;
  } else if (calibrationVarIdx == 2) {
    //calibrationVarValue = AppSettings::calibrationSettings.yAxisMin;
    //calibrationVarLabel[0] = 'Y';calibrationVarLabel[1] = 'M';calibrationVarLabel[2] = 'I';calibrationVarLabel[3] = 'N';calibrationVarLabel[4] = ' ';calibrationVarLabel[5] = 0;
  } else if (calibrationVarIdx == 3) {
    //calibrationVarValue = AppSettings::calibrationSettings.yAxisMax;
    //calibrationVarLabel[0] = 'Y';calibrationVarLabel[1] = 'M';calibrationVarLabel[2] = 'A';calibrationVarLabel[3] = 'X';calibrationVarLabel[4] = ' ';calibrationVarLabel[5] = 0;
  } else if (calibrationVarIdx == 4) {
    //calibrationVarValue = AppSettings::calibrationSettings.zAxisMin;
    //calibrationVarLabel[0] = 'Z';calibrationVarLabel[1] = 'M';calibrationVarLabel[2] = 'I';calibrationVarLabel[3] = 'N';calibrationVarLabel[4] = ' ';calibrationVarLabel[5] = 0;
  } else if (calibrationVarIdx == 5) {
    //calibrationVarValue = AppSettings::calibrationSettings.zAxisMax;
    //calibrationVarLabel[0] = 'Z';calibrationVarLabel[1] = 'M';calibrationVarLabel[2] = 'A';calibrationVarLabel[3] = 'X';calibrationVarLabel[4] = ' ';calibrationVarLabel[5] = 0;
  }
}

void AppLogic::setupCalibrationMessage(bool capturing, bool calibrating) {
  if (capturing) {
    calibrationMessage[0] = 'S';calibrationMessage[1] = 't';calibrationMessage[2] = 'e';calibrationMessage[3] = 'p';calibrationMessage[4] = ' ';calibrationMessage[5] = calibrationVarIdx+'1';calibrationMessage[6] = ' ';
    calibrationMessage[7] = 'h';calibrationMessage[8] = 'o';calibrationMessage[9] = 'l';calibrationMessage[10] = 'd';calibrationMessage[11] = '\0';
  } else if (calibrating) {
    calibrationMessage[0] = 'C';calibrationMessage[1] = 'a';calibrationMessage[2] = 'l';calibrationMessage[3] = 'i';calibrationMessage[4] = 'b';calibrationMessage[5] = 'r';
    calibrationMessage[6] = 'a';calibrationMessage[7] = 't';calibrationMessage[8] = 'i';calibrationMessage[9] = 'n';calibrationMessage[10] = 'g';calibrationMessage[11] = '.';calibrationMessage[12] = '.';calibrationMessage[13] = '.';calibrationMessage[14] = '\0';
  } else {
    calibrationMessage[0] = 'S';calibrationMessage[1] = 't';calibrationMessage[2] = 'e';calibrationMessage[3] = 'p';calibrationMessage[4] = ' ';calibrationMessage[5] = calibrationVarIdx+'1';calibrationMessage[6] = ' ';
    calibrationMessage[7] = 'r';calibrationMessage[8] = 'e';calibrationMessage[9] = 'a';calibrationMessage[10] = 'd';calibrationMessage[11] = 'y';calibrationMessage[12] = '\0';
  }
  AppMenu::update();
}

