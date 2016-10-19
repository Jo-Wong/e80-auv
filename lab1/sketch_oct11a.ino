#include <SdFat.h>
#include <SPI.h>
#include <MyLogger.h>
#include <MotorDriver.h>
#include <Params.h>

#define LOOP_INTERVAL 500000
IntervalTimer controlTimer;

SdFat sd;
SdFile file;
MyLogger logger(sd, file);
MotorDriver motorDriver(MOTOR_L_FORWARD,MOTOR_L_REVERSE,MOTOR_R_FORWARD,MOTOR_R_REVERSE,MOTOR_V_FORWARD,MOTOR_V_REVERSE); 

#define SEQ_LEN 11
size_t curridx = 0;
int seq_l[SEQ_LEN] = {10,25,0,50,0,75,0,100,0,125,0}; 
int seq_r[SEQ_LEN] = {10,25,0,50,0,75,0,100,0,125,0};
int seq_v[SEQ_LEN] = {10,25,0,50,0,75,0,100,0,125,0};
int seq_t[SEQ_LEN] = {2,2,2,2,2,2,2,2,2,2,2};
unsigned long last_trans = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(2000);
  Serial.print("Logger: initializing sd card");

  if(!sd.begin(10, SPI_FULL_SPEED)) {
    Serial.print("card failed");
    return;
  }
  Serial.print("card initialized");
  logger.include(&motorDriver);
  logger.init();

  pinMode(MOTOR_L_FORWARD,OUTPUT);
  pinMode(MOTOR_L_REVERSE,OUTPUT);
  pinMode(MOTOR_R_FORWARD,OUTPUT);
  pinMode(MOTOR_R_REVERSE,OUTPUT);
  pinMode(MOTOR_V_FORWARD,OUTPUT);
  pinMode(MOTOR_V_REVERSE,OUTPUT);
  
  Serial.print("start control loop");
  Serial.print("Press any character to stop logging");
  last_trans = millis();
  controlTimer.begin(controlLoop, LOOP_INTERVAL);
}

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

void loop() {
  // put your main code here, to run repeatedly:
  logger.write();
  
}
