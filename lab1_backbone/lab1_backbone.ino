/******
 * lab1_backbone
 * Author: Apoorva Sharma (asharma@hmc.edu)
 * Edited by: Josephine Wong (jowong@hmc.edu)
 * Created: 20 Jun 2016
 * Runs motors at various speeds, and records speeds as well
 *******/

/* Libraries */
// Logging
#include <SdFat.h>
#include <SPI.h>
#include <MyLogger.h>

// Motor
#include <MotorDriver.h>
#include <Params.h>

/* Timing Variables */
#define LOOP_INTERVAL 500000
IntervalTimer controlTimer;

/* Logger and MotorDriver Objects */
SdFat sd;
SdFile file;
MyLogger logger(sd, file);
MotorDriver motorDriver(MOTOR_L_FORWARD,MOTOR_L_REVERSE,MOTOR_R_FORWARD,MOTOR_R_REVERSE); 

// PWM Sequence
#define SEQ_LEN 11
size_t curridx = 0;
int seq_l[SEQ_LEN] = {10,25,0,50,0,75,0,100,0,125,0}; 
int seq_r[SEQ_LEN] = {10,25,0,50,0,75,0,100,0,125,0};
int seq_t[SEQ_LEN] = {2,2,2,2,2,2,2,2,2,2,2};
unsigned long last_trans = 0;

/**************************************************************************/
void setup() {
  Serial.begin(9600);
  delay(2000); // Wait to ensure computer monitor is ready
  Serial.print("Logger: initializing sd card");

  // see if the card is present and can be initialized:
  if(!sd.begin(10, SPI_FULL_SPEED)) {
    Serial.print("card failed");
    return;
  }
  Serial.print("card initialized");
  
  /* Initialize the Logger */  
  logger.include(&motorDriver);
  logger.init();

  /* Initialize the motor pins */
  pinMode(MOTOR_L_FORWARD,OUTPUT);
  pinMode(MOTOR_L_REVERSE,OUTPUT);
  pinMode(MOTOR_R_FORWARD,OUTPUT);
  pinMode(MOTOR_R_REVERSE,OUTPUT);
  
  Serial.print("start control loop");
  Serial.print("Press any character to stop logging");
  last_trans = millis();
  controlTimer.begin(controlLoop, LOOP_INTERVAL);
}

/**************************************************************************/
void controlLoop(void) {
  unsigned long current_time = millis();
  Serial.print(current_time);
  
  // handle state transitions
  if (current_time - last_trans >= seq_t[curridx]*1000) {
    if (curridx < SEQ_LEN - 1) {
      curridx++;
      last_trans = current_time;
    }
    Serial.print("switching to state "); Serial.println(curridx);
  }
  
  motorDriver.left = seq_l[curridx];
  motorDriver.right = seq_r[curridx];
  motorDriver.vertical = seq_v[curridx];
  motorDriver.apply();
  motorDriver.printState();
  
  logger.log(); 
}

/**************************************************************************/
void loop() {
  // only work on writing the files
  logger.write();
  
}
