#include <Wire.h>
#include <Motoron.h>
#include <QTRSensors.h>

MotoronI2C motors;
QTRSensors qtr;

const uint8_t qtrPins[] = {2, 3, 4, 5, 6};
uint16_t sensorValues[5];
const int FRONT_SENSOR = 11;
const int BASE_SPEED = 600; 

void setup() {
  Wire.begin();
  motors.reinitialize();
  motors.clearResetFlag();
  motors.setNominalConfig();
  qtr.setTypeRC();
  qtr.setSensorPins(qtrPins, 5);
  pinMode(FRONT_SENSOR, INPUT);
}

void loop() {
  if (digitalRead(FRONT_SENSOR) == LOW) {
    motors.setSpeed(1, 0);
    motors.setSpeed(2, 0);
    return;
  }

  uint16_t position = qtr.readLineBlack(sensorValues);

  // Depend on JMotor data
  // Pseudocode approach：
  // if (getDistance() >= 250) { 
  //    make90DegreeTurn(); 
  //    resetDistance();
  // }

  int error = (int)position - 2000; 
  int leftSpeed = BASE_SPEED + (error / 5);
  int rightSpeed = BASE_SPEED - (error / 5);

  motors.setSpeed(1, leftSpeed);
  motors.setSpeed(2, rightSpeed);
}
