#ifndef APPLOGIC_H_
#define APPLOGIC_H_

#include "AppMenu.h"
#include "GForceCalibration.h"

class AppLogic {
  friend class AppMenu;
  
  public:
    static bool countdownRunning;
    static unsigned long countdownStart;
    static unsigned int buzzerDelay;
    static bool buzzerRunning;
    static bool startStopModeRunning;
    static unsigned long buzzerStarted;
    static float elapsedTime;
    static unsigned long lastElapsedTimeMenuUpdate;
    static unsigned short currentGx;
    static unsigned short currentGy;
    static unsigned short currentGz;
    static unsigned long lastGreadingsUpdate;
    static char* calibrationMessage;
    static bool calibrationRunning;
    static char* calibrationVarLabel;
    static unsigned short calibrationVarIdx;
    
    static void setup();
    static void loop();
    
  private:
    static void readGforces();
    static void toggleCalibration();
    static void startCountdown();
    static void incMinDelay();
    static void decMinDelay();
    static void incMaxDelay();
    static void decMaxDelay();
    static void startStopModeUp();
    static void startStopModeDown();
    static void startBuzzer();
    static void checkBuzzerStop();
    static void stopBuzzer();
    static void updateCalibrationVal();
    static void setupCalibrationMessage(bool capturing, bool calibrating);
    static GForceCalibration forceCalibration;
};

#endif
