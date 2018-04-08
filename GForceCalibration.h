/*
# copyright 2011 by Rolfe Schmidt
# This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License. 
# To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
# letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
#
# Available at http://rolfeschmidt.com/mathtools/skimetrics/adxl_gn_calibration.pde
#
# Described at http://wp.me/p1CQVj-1k
*/

#include <Wire.h> 
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidMenu.h>
#include <buttons.h>
#include <Arduino.h>

#define NUM_READINGS 6
#define SAMPLE_SIZE 32

class GForceCalibration {
  public:
    GForceCalibration(int xpin, int ypin, int zpin);
    void setup();
    void takeSample(unsigned short nSamp);
    void calibrate();
    
  private:
    int m_xpin;
    int m_ypin;
    int m_zpin;
    unsigned int *data;             // dynamically allocated array for data storage.  Don't let it get too big!
    int samp_capacity = 0;          // the capacity of the sample data array
    int n_samp = 0;                 // Number of samples used for calibration
    
    float beta[6];                  // parameters for model.  beta[0], beta[1], and beta[2] are the 0-G marks (about 512),
                                    // while beta[3], beta[4], and beta[5] are the scaling factors.  So, e.g., if xpin reads
                                    // value x, number of G's in the x direction in beta[3]*(x - beta[0]).
                                      
    float JS[6][6];                 // matrices for Gauss-Newton computations
    float dS[6];
    float delta[6];      

    void reset_calibration_matrices();
    void update_calibration_matrices(const unsigned int* data);
    void calibrate_model_matrices();
    void find_delta();
    void calibrate_model();
    void take_sample(unsigned int* sample_out);
    void compute_calibration_matrices();
};
/*
void loop()
{
  
  //Check if the button was pressed.  Toggle data reporting if it was.
  int val;
  val = digitalRead(takeSamplePin);
  if (val==LOW) {
    //Serial.println("Take sample");
    delay(200);
    take_sample(data + 3*(n_samp%samp_capacity));
    n_samp++;
  }
  
  val = digitalRead(calibratePin);
  if (val==LOW) {
    Serial.println("Calibrate");
    calibrate_model();
  }
}
*/

