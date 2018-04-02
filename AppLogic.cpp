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
char* AppLogic::calibrationVarLabel;
unsigned short AppLogic::calibrationVarIdx = 0;
unsigned short AppLogic::calibrationVarValue;
GForceCalibration AppLogic::forceCalibration = GForceCalibration(A1, A2, A3);

void AppLogic::setup() {  
  Serial.begin(9600);
  
  AppLogic::calibrationVarLabel = new char[6];
  
  AppLogic::calibrationMessage = new char[17];
  
  setupCalibrationMessage(false);  

  pinMode(buzzerPin, OUTPUT);  
  
  updateCalibrationVal();
  
  AppMenu::setupMenu();
  
  AppButtons::setupButtons();

  AppSettings::setupSettings();
}

void AppLogic::loop() {
  readGforces();
  
  if (calibrationRunning) {
    calibrationIteration();
  }
  
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

void AppLogic::incBrowseCalibration() {
  if (calibrationVarIdx == 5) {
    calibrationVarIdx = 0;
  } else {
    calibrationVarIdx++;
  }
  AppLogic::updateCalibrationVal();
  AppMenu::update();
}

void AppLogic::decBrowseCalibration() {
  if (calibrationVarIdx == 0) {
    calibrationVarIdx = 5;
  } else {
    calibrationVarIdx--;
  }
  AppLogic::updateCalibrationVal();
  AppMenu::update();
}

void AppLogic::toggleCalibration() {
  calibrationRunning = !calibrationRunning;
  AppLogic::setupCalibrationMessage(calibrationRunning);
  AppMenu::update();
  if (!calibrationRunning){
    AppSettings::saveSettings();
  } else {
    AppSettings::calibrationSettings.xAxisMin = 65000;
    AppSettings::calibrationSettings.xAxisMax = 0;
    AppSettings::calibrationSettings.yAxisMin = 65000;
    AppSettings::calibrationSettings.yAxisMax = 0;
    AppSettings::calibrationSettings.zAxisMin = 65000;
    AppSettings::calibrationSettings.zAxisMax = 0;
  }
}

void AppLogic::calibrationIteration() {
  AppSettings::calibrationSettings.xAxisMin = min(AppSettings::calibrationSettings.xAxisMin, currentGx);
  AppSettings::calibrationSettings.xAxisMax = max(AppSettings::calibrationSettings.xAxisMax, currentGx);
  AppSettings::calibrationSettings.yAxisMin = min(AppSettings::calibrationSettings.yAxisMin, currentGy);
  AppSettings::calibrationSettings.yAxisMax = max(AppSettings::calibrationSettings.yAxisMax, currentGy);
  AppSettings::calibrationSettings.zAxisMin = min(AppSettings::calibrationSettings.zAxisMin, currentGz);
  AppSettings::calibrationSettings.zAxisMax = max(AppSettings::calibrationSettings.zAxisMax, currentGz);
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
    calibrationVarValue = AppSettings::calibrationSettings.xAxisMin;
    calibrationVarLabel[0] = 'X';calibrationVarLabel[1] = 'M';calibrationVarLabel[2] = 'I';calibrationVarLabel[3] = 'N';calibrationVarLabel[4] = ' ';calibrationVarLabel[5] = 0;
  } else if (calibrationVarIdx == 1) {
    calibrationVarValue = AppSettings::calibrationSettings.xAxisMax;
    calibrationVarLabel[0] = 'X';calibrationVarLabel[1] = 'M';calibrationVarLabel[2] = 'A';calibrationVarLabel[3] = 'X';calibrationVarLabel[4] = ' ';calibrationVarLabel[5] = 0;
  } else if (calibrationVarIdx == 2) {
    calibrationVarValue = AppSettings::calibrationSettings.yAxisMin;
    calibrationVarLabel[0] = 'Y';calibrationVarLabel[1] = 'M';calibrationVarLabel[2] = 'I';calibrationVarLabel[3] = 'N';calibrationVarLabel[4] = ' ';calibrationVarLabel[5] = 0;
  } else if (calibrationVarIdx == 3) {
    calibrationVarValue = AppSettings::calibrationSettings.yAxisMax;
    calibrationVarLabel[0] = 'Y';calibrationVarLabel[1] = 'M';calibrationVarLabel[2] = 'A';calibrationVarLabel[3] = 'X';calibrationVarLabel[4] = ' ';calibrationVarLabel[5] = 0;
  } else if (calibrationVarIdx == 4) {
    calibrationVarValue = AppSettings::calibrationSettings.zAxisMin;
    calibrationVarLabel[0] = 'Z';calibrationVarLabel[1] = 'M';calibrationVarLabel[2] = 'I';calibrationVarLabel[3] = 'N';calibrationVarLabel[4] = ' ';calibrationVarLabel[5] = 0;
  } else if (calibrationVarIdx == 5) {
    calibrationVarValue = AppSettings::calibrationSettings.zAxisMax;
    calibrationVarLabel[0] = 'Z';calibrationVarLabel[1] = 'M';calibrationVarLabel[2] = 'A';calibrationVarLabel[3] = 'X';calibrationVarLabel[4] = ' ';calibrationVarLabel[5] = 0;
  }
}

void AppLogic::setupCalibrationMessage(bool calibrationRunning) {
  if (calibrationRunning) {
    calibrationMessage[0] = 'U';
    calibrationMessage[1] = 'P';
    calibrationMessage[2] = '/';
    calibrationMessage[3] = 'D';
    calibrationMessage[4] = 'O';
    calibrationMessage[5] = 'W';
    calibrationMessage[6] = 'N';
    calibrationMessage[7] = ' ';
    calibrationMessage[8] = 't';
    calibrationMessage[9] = 'o';
    calibrationMessage[10] = ' ';
    calibrationMessage[11] = 's';
    calibrationMessage[12] = 't';
    calibrationMessage[13] = 'o';
    calibrationMessage[14] = 'p';
    calibrationMessage[15] = 0;
  } else {
    calibrationMessage[0] = 'U';
    calibrationMessage[1] = 'P';
    calibrationMessage[2] = '/';
    calibrationMessage[3] = 'D';
    calibrationMessage[4] = 'O';
    calibrationMessage[5] = 'W';
    calibrationMessage[6] = 'N';
    calibrationMessage[7] = ' ';
    calibrationMessage[8] = 't';
    calibrationMessage[9] = 'o';
    calibrationMessage[10] = ' ';
    calibrationMessage[11] = 's';
    calibrationMessage[12] = 't';
    calibrationMessage[13] = 'a';
    calibrationMessage[14] = 'r';
    calibrationMessage[15] = 't';
    calibrationMessage[16] = 0;
  }
}

