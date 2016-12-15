/********
 * Lab 1 Sketch
 * Author: Apoorva Sharma (asharma@hmc.edu) 
 * Edited by: Josephine Wong (jowong@hmc.edu)
 * Created: 20 Jun 2016
 * Runs the motors at various speeds for specific time intervals
 *********/

/* Libraries */
#include <MotorDriver.h>
#include <Params.h>
#include <SdFat.h>
#include <MyLogger.h>
#include <SPI.h>
#include <Wire.h>

/* Global Variables */
#define LOOP_INTERVAL 100 // in ms
IntervalTimer controlTimer;
MotorDriver motorDriver(MOTOR_L_FORWARD,MOTOR_L_REVERSE,MOTOR_R_FORWARD,MOTOR_R_REVERSE,MOTOR_V_FORWARD,MOTOR_V_REVERSE); 

// Logger
SdFat sd;
SdFile file;
MyLogger logger(sd,file);

//-------------- Todo: Motor Sequence ----------//
#define SEQ_LEN 11
size_t curridx = 0;
int seq_l[SEQ_LEN] = {10,25,0,50,0,75,0,100,0,125,0}; 
int seq_r[SEQ_LEN] = {10,25,0,50,0,75,0,100,0,125,0};
int seq_v[SEQ_LEN] = {10,25,0,50,0,75,0,100,0,125,0};
int seq_t[SEQ_LEN] = {2,2,2,2,2,2,2,2,2,2,2};
unsigned long last_trans = 0;

// setup(): initializes logger and motor pins
void setup() {
  Serial.begin(115200);
  delay(2000); // Wait to ensure computer monitor is ready
  Serial.println(F("Serial connection started")); 
  Serial.println("");

  Serial.print("\nLogger: Initializing SD card...");

  // check if the card is present and can be initialized
  if(!sd.begin(SD_CHIP_SELECT, SPI_FULL_SPEED)) {
    Serial.println("Card failed, or not present");
    // don't do anything more
    return;
  }
  Serial.println("Card initialized");

  /* Initialize the Logger */
  logger.include(&motorDriver);
  logger.init();

  //-------Todo: initialize motor pins--------//
  pinMode(MOTOR_L_FORWARD,OUTPUT);
  pinMode(MOTOR_L_REVERSE,OUTPUT);
  pinMode(MOTOR_R_FORWARD,OUTPUT);
  pinMode(MOTOR_R_REVERSE,OUTPUT);
  pinMode(MOTOR_V_FORWARD,OUTPUT);
  pinMode(MOTOR_V_REVERSE,OUTPUT);

  Serial.println("starting control loop");
  Serial.println("Press any character to stop logging");
  last_trans = millis();
  controlTimer.begin(controlLoop, LOOP_INTERVAL*1000);
}

// controlLoop(): updates motor output at every LOOP_INTERVAL 
void controlLoop(void) {
  unsigned long current_time = millis();
  
  //-------Todo: write loop that handles state transitions----//
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

// loop(): write the buffered data to the sd card
void loop() {
  logger.write();
}
