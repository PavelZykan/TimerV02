#include "AppButtons.h"

Button AppButtons::left = Button(OneShot);
Button AppButtons::up = Button(OneShot);
Button AppButtons::right = Button(OneShot);
Button AppButtons::down = Button(OneShot);

void AppButtons::setupButtons() {
  AppButtons::left.assign(7);
  AppButtons::left.turnOnPullUp();
  AppButtons::left.check();
  AppButtons::up.assign(9);
  AppButtons::up.turnOnPullUp();
  AppButtons::up.check();
  AppButtons::down.assign(8);
  AppButtons::down.turnOnPullUp();
  AppButtons::down.check();
  AppButtons::right.assign(10);
  AppButtons::right.turnOnPullUp();  
  AppButtons::right.check();
};
