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
/* Create integer arrays for left, right, and vertical motors and time
 * Motor values should be between -255 and 255
 * Each index will represent a state (e.g. index = 0 means state0)
 */

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
  /* You should use the pinMode() function */
  pinMode(LOOP_LED,OUTPUT);

  Serial.println("starting control loop");
  Serial.println("Press any character to stop logging");
  controlTimer.begin(controlLoop, LOOP_INTERVAL*1000);
}

// controlLoop(): updates motor output at every LOOP_INTERVAL 
void controlLoop(void) {
  //-------Todo: write loop that handles state transitions----//
  /* You can get the current time using millis() */
  /* Use motorDriver object to set the motor values */

  digitalWrite(LOOP_LED,0);
  logger.log();
}

// loop(): write the buffered data to the sd card
void loop() {
  digitalWrite(LOOP_LED,1);
  logger.write();
}

