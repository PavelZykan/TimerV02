#include "GForceCalibration.h"

GForceCalibration::GForceCalibration(int xpin, int ypin, int zpin) {
  m_xpin = xpin;
  m_ypin = ypin;
  m_zpin = zpin;
}

void GForceCalibration::calibrate() {
  
}

void GForceCalibration::takeSample() {
  
}

void GForceCalibration::setup() {
  //set up the pins
  pinMode(m_xpin, INPUT);
  pinMode(m_ypin, INPUT);
  pinMode(m_zpin, INPUT);
  
  //initialize to handle 20 samples
  samp_capacity = 20;
  data = (unsigned int*)malloc(samp_capacity*3*sizeof(unsigned int));
  
  reset_calibration_matrices();
  
  //initialize beta to something reasonable
  beta[0] = beta[1] = beta[2] = 512.0;
  beta[3] = beta[4] = beta[5] = 0.0095; 
}


//Gauss-Newton functions

void GForceCalibration::reset_calibration_matrices() {
  int j,k;
  for(j=0;j<6;++j) {
     dS[j] = 0.0;
     for(k=0;k<6;++k) {
       JS[j][k] = 0.0;
     }
  }      
}

void GForceCalibration::update_calibration_matrices(const unsigned int* data) {
    int j, k;
    float dx, b;
    float residual = 1.0;
    float jacobian[6];
    
    for(j=0;j<3;++j) {
      b = beta[3+j];
      dx = ((float)data[j])/SAMPLE_SIZE - beta[j];
      residual -= b*b*dx*dx;
      jacobian[j] = 2.0*b*b*dx;
      jacobian[3+j] = -2.0*b*dx*dx;
    }
    
    for(j=0;j<6;++j) {
       dS[j] += jacobian[j]*residual;
       for(k=0;k<6;++k) {
         JS[j][k] += jacobian[j]*jacobian[k];
       }
    }    
}

void GForceCalibration::find_delta() {
  //Solve 6-d matrix equation JS*x = dS
  //first put in upper triangular form
  int i,j,k;
  float mu;
  
  //make upper triangular
  for(i=0;i<6;++i) {
    //eliminate all nonzero entries below JS[i][i]
    for(j=i+1;j<6;++j) {
      mu = JS[i][j]/JS[i][i];
      if(mu != 0.0) {
        dS[j] -= mu*dS[i];
        for(k=j;k<6;++k) {
         JS[k][j] -= mu*JS[k][i];
        } 
      }
    }
  }
  
  //back-substitute
  for(i=5;i>=0;--i) {
    dS[i] /= JS[i][i];
    JS[i][i] = 1.0;
    for(j=0;j<i;++j) {
      mu = JS[i][j];
      dS[j] -= mu*dS[i];
      JS[i][j] = 0.0;
    }
  }
  
  for(i=0;i<6;++i) {
    delta[i] = dS[i];
  }
}

void GForceCalibration::compute_calibration_matrices() {
    int i, j, k;
    float dx, b;
  
    reset_calibration_matrices();
    int ub = n_samp < samp_capacity ? n_samp : samp_capacity;
    for(i=0;i<ub;i++) {    
      update_calibration_matrices(data+3*i);
    }    
}

//pass in a length-3 array where the data will be written
void GForceCalibration::take_sample(unsigned int* sample_out) {
  int i=0;
  int first_pass_size = 5;
  int success = 0;  
  
  while (success == 0) {
    //First, run through 32 samples and accumulate the mean and variance.
    //Make all variables longs because we will do some aritmetic that 
    // will overflow an int.
    unsigned long sum[] = {0,0,0};
    unsigned long sum_squares[] = {0,0,0};
    unsigned long variance[] = {0,0,0};
    unsigned long x,y,z;
    for(i=0;i< (1<<first_pass_size);++i) {
      x= analogRead(m_xpin);
      delay(1);
      y= analogRead(m_ypin);
      delay(1);
      z= analogRead(m_zpin);
      delay(18); //sample at 50 Hz
      
      sum[0] += x;
      sum[1] += y;
      sum[2] += z;
      
      sum_squares[0] += x*x;
      sum_squares[1] += y*y;
      sum_squares[2] += z*z;
    }
    
    //now compute the variance onh each axis. Don't divide by 32 -- keep the extra digits
    //around to reduce quantization errors.  So really computing variance*32 here. Also make sure it is > 0.
    for(i=0;i<3; i++) {
      /*Serial.print("Sum squares: ");
      Serial.print(sum_squares[i]);
      Serial.print(" Sum, squared: ");
      Serial.print((sum[i]*sum[i]) /32);
      Serial.println();
      */
      variance[i] = 1+ sum_squares[i] - (sum[i]*sum[i])/32;
    }
    
    
    //with mean and variance in place, start collecting real samples but filter out outliers.
    //Track the success rate and start over if we get too many fails.
    unsigned int success_count = 0;
    unsigned int fail_count = 0;
    i=0;
    sample_out[0] = sample_out[2] = sample_out[1] = 0;
    
    
    while(i < SAMPLE_SIZE) {
      
      //take a reading
      x= analogRead(m_xpin);
      delay(1);
      y= analogRead(m_ypin);
      delay(1);
      z= analogRead(m_zpin);
      delay(18); //sample at 50 Hz
      
      unsigned long dx = x*32 - sum[0];
      unsigned long dy = y*32 - sum[1];
      unsigned long dz = z*32 - sum[2];
      
      //check to see if it is any good (within 3 std deviations)
      if((dx*dx)/32 < 9*variance[0]
          &&(dy*dy)/32 < 9*variance[1]
          &&(dz*dz)/32 < 9*variance[2]) {
        success_count++;
        sample_out[0] += x;
        sample_out[1] += y;
        sample_out[2] += z;
        
        ++i;
      } else {        
        fail_count++;
      }
      
      if(fail_count > success_count && i > 10) {
        //we're failing too much, start over!
        Serial.println("#Sample fail 1");
        break;
      }       
    }      
    
    //if we got our samples, mark the success.  Otherwise we'll start over.
    if (i == SAMPLE_SIZE) {
      success = 1;  
    }
  }
}

void GForceCalibration::calibrate_model() {
  int i;
  float eps = 0.000000001;
  int num_iterations = 20;
  float change = 100.0;
  while (--num_iterations >=0 && change > eps) {
    compute_calibration_matrices();
    find_delta();
    change = delta[0]*delta[0] + delta[0]*delta[0] + delta[1]*delta[1] + delta[2]*delta[2] + delta[3]*delta[3]/(beta[3]*beta[3]) + delta[4]*delta[4]/(beta[4]*beta[4]) + delta[5]*delta[5]/(beta[5]*beta[5]); 
    
    for(i=0;i<6;++i) {
      beta[i] -= delta[i];
    }
    
    reset_calibration_matrices();
    /*
    Serial.print( "Num iterations: ");
    Serial.print(20 - num_iterations);
    Serial.print( " change: ");
    Serial.println( change, 10);
    */
  }
  
  Serial.print("\n");
  for(i=0;i<6;++i) {
    Serial.print(beta[i], 7);
  }
   Serial.println();  
}

