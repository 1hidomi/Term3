#include <Arduino.h>
#include <Wire.h>
#include <Motoron.h>

MotoronI2C mc;

const uint8_t MOTOR_1 = 1;  // right front
const uint8_t MOTOR_2 = 2;  // right rear
const uint8_t MOTOR_3 = 3;  // left rear
const uint8_t MOTOR_4 = 4;  // left front

// Encoder wiring you provided:
// M1 -> A:D18 B:D19, M2 -> A:D2 B:D3, M3 -> A:D4 B:D5, M4 -> A:D8 B:D9
const uint8_t ENC_A[4] = {18, 2, 4, 8};
const uint8_t ENC_B[4] = {19, 3, 5, 9};
volatile long encCount[4] = {0, 0, 0, 0};

// Reflectance pins moved away from encoder pins.
const uint8_t SENSOR_PINS[3] = {10, 11, 12};  // right, mid, left
const uint8_t IDX_RIGHT = 0;
const uint8_t IDX_MID = 1;
const uint8_t IDX_LEFT = 2;
unsigned long sensorDuration[3] = {0, 0, 0};

const uint16_t START_THRESHOLD_US = 50;
const int16_t BASE_SPEED = 260;
const int16_t TURN_OFFSET = 120;
const int16_t MAX_SPEED = 600;
const int16_t SELF_TEST_SPEED = 180;
const unsigned long SELF_TEST_MS = 2000;

void encoderISR0() { encCount[0] += digitalRead(ENC_B[0]) ? 1 : -1; }
void encoderISR1() { encCount[1] += digitalRead(ENC_B[1]) ? 1 : -1; }
void encoderISR2() { encCount[2] += digitalRead(ENC_B[2]) ? 1 : -1; }
void encoderISR3() { encCount[3] += digitalRead(ENC_B[3]) ? 1 : -1; }

void setDrive(int16_t leftSpeed, int16_t rightSpeed)
{
  leftSpeed = constrain(leftSpeed, -MAX_SPEED, MAX_SPEED);
  rightSpeed = constrain(rightSpeed, -MAX_SPEED, MAX_SPEED);

  mc.setSpeed(MOTOR_4, leftSpeed);
  mc.setSpeed(MOTOR_3, leftSpeed);
  mc.setSpeed(MOTOR_1, rightSpeed);
  mc.setSpeed(MOTOR_2, rightSpeed);
}

void readReflectance()
{
  for (uint8_t i = 0; i < 3; i++) {
    pinMode(SENSOR_PINS[i], OUTPUT);
    digitalWrite(SENSOR_PINS[i], HIGH);
    delayMicroseconds(10);
    pinMode(SENSOR_PINS[i], INPUT);

    unsigned long startTime = micros();
    while (digitalRead(SENSOR_PINS[i]) == HIGH && (micros() - startTime) < 3000) {
    }
    sensorDuration[i] = micros() - startTime;
  }
}

void setup()
{
  Serial.begin(9600);
  delay(1000);

  // 注意：使用 Wire1
  Wire1.begin();
  mc.setBus(&Wire1);

  mc.reinitialize();
  mc.disableCrc();
  mc.clearResetFlag();

  for (uint8_t i = 0; i < 4; i++) {
    pinMode(ENC_A[i], INPUT_PULLUP);
    pinMode(ENC_B[i], INPUT_PULLUP);
  }
  attachInterrupt(digitalPinToInterrupt(ENC_A[0]), encoderISR0, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_A[1]), encoderISR1, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_A[2]), encoderISR2, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_A[3]), encoderISR3, RISING);

  mc.setMaxAcceleration(MOTOR_1, 300);
  mc.setMaxDeceleration(MOTOR_1, 500);
  mc.setMaxAcceleration(MOTOR_2, 300);
  mc.setMaxDeceleration(MOTOR_2, 500);
  mc.setMaxAcceleration(MOTOR_3, 300);
  mc.setMaxDeceleration(MOTOR_3, 500);
  mc.setMaxAcceleration(MOTOR_4, 300);
  mc.setMaxDeceleration(MOTOR_4, 500);

  Serial.println("[NEW CODE] Startup motor self-test begins...");
  setDrive(SELF_TEST_SPEED, SELF_TEST_SPEED);
  delay(SELF_TEST_MS);
  setDrive(0, 0);
  Serial.println("[NEW CODE] Self-test done. Reflectance control active.");
}

void loop()
{
  readReflectance();

  bool rightOnLine = sensorDuration[IDX_RIGHT] > START_THRESHOLD_US;
  bool midOnLine = sensorDuration[IDX_MID] > START_THRESHOLD_US;
  bool leftOnLine = sensorDuration[IDX_LEFT] > START_THRESHOLD_US;

  int16_t leftSpeed = 0;
  int16_t rightSpeed = 0;

  if (midOnLine) {
    leftSpeed = BASE_SPEED;
    rightSpeed = BASE_SPEED;

    if (leftOnLine && !rightOnLine) {
      leftSpeed = BASE_SPEED - TURN_OFFSET;
      rightSpeed = BASE_SPEED + TURN_OFFSET;
    } else if (rightOnLine && !leftOnLine) {
      leftSpeed = BASE_SPEED + TURN_OFFSET;
      rightSpeed = BASE_SPEED - TURN_OFFSET;
    }
  }

  setDrive(leftSpeed, rightSpeed);

  Serial.print("Ref L/M/R: ");
  Serial.print(sensorDuration[IDX_LEFT]);
  Serial.print("/");
  Serial.print(sensorDuration[IDX_MID]);
  Serial.print("/");
  Serial.print(sensorDuration[IDX_RIGHT]);
  Serial.print(" | Enc 1/2/3/4: ");
  Serial.print(encCount[0]);
  Serial.print("/");
  Serial.print(encCount[1]);
  Serial.print("/");
  Serial.print(encCount[2]);
  Serial.print("/");
  Serial.print(encCount[3]);
  Serial.print(" | Speed L/R: ");
  Serial.print(leftSpeed);
  Serial.print("/");
  Serial.println(rightSpeed);

  delay(30);
}
