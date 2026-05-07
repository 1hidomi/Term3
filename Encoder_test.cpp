#include <Arduino.h>
#include <Wire.h>
#include <Motoron.h>

MotoronI2C mc;

// 你的实际车轮布局
const uint8_t RIGHT_FRONT = 1;  // M1 右前
const uint8_t RIGHT_REAR  = 2;  // M2 右后
const uint8_t LEFT_REAR   = 3;  // M3 左后 / 左下
const uint8_t LEFT_FRONT  = 4;  // M4 左前 / 左上

// 如果某个电机方向反了，就把对应的 1 改成 -1
const int8_t DIR_RIGHT_FRONT = 1;
const int8_t DIR_RIGHT_REAR  = 1;
const int8_t DIR_LEFT_REAR   = 1;
const int8_t DIR_LEFT_FRONT  = 1;

// 顺时针转圈：左边快，右边慢
const int16_t LEFT_SPEED  = 550;
const int16_t RIGHT_SPEED = 250;

// 跑多久，先试 6 秒
const unsigned long RUN_TIME_MS = 6000;

unsigned long startTime = 0;
bool finished = false;

void stopAllMotors()
{
  mc.setSpeed(RIGHT_FRONT, 0);
  mc.setSpeed(RIGHT_REAR, 0);
  mc.setSpeed(LEFT_REAR, 0);
  mc.setSpeed(LEFT_FRONT, 0);
}

void turnClockwise()
{
  // 右边两个轮子慢：M1 + M2
  mc.setSpeed(RIGHT_FRONT, RIGHT_SPEED * DIR_RIGHT_FRONT);
  mc.setSpeed(RIGHT_REAR,  RIGHT_SPEED * DIR_RIGHT_REAR);

  // 左边两个轮子快：M3 + M4
  mc.setSpeed(LEFT_REAR,   LEFT_SPEED * DIR_LEFT_REAR);
  mc.setSpeed(LEFT_FRONT,  LEFT_SPEED * DIR_LEFT_FRONT);
}

void setup()
{
  Serial.begin(115200);
  delay(1000);

  // 重点：继续用 Wire1
  Wire1.begin();
  mc.setBus(&Wire1);

  mc.reinitialize();
  mc.disableCrc();
  mc.clearResetFlag();

  mc.setMaxAcceleration(RIGHT_FRONT, 400);
  mc.setMaxDeceleration(RIGHT_FRONT, 600);

  mc.setMaxAcceleration(RIGHT_REAR, 400);
  mc.setMaxDeceleration(RIGHT_REAR, 600);

  mc.setMaxAcceleration(LEFT_REAR, 400);
  mc.setMaxDeceleration(LEFT_REAR, 600);

  mc.setMaxAcceleration(LEFT_FRONT, 400);
  mc.setMaxDeceleration(LEFT_FRONT, 600);

  stopAllMotors();

  Serial.println("Clockwise circle test with 4 motors");
  Serial.println("M1=Right Front, M2=Right Rear, M3=Left Rear, M4=Left Front");
  Serial.println("2 seconds later start...");
  delay(2000);

  startTime = millis();
}

void loop()
{
  if (finished)
  {
    stopAllMotors();
    delay(100);
    return;
  }

  unsigned long elapsed = millis() - startTime;

  if (elapsed < RUN_TIME_MS)
  {
    turnClockwise();

    Serial.print("Turning clockwise: ");
    Serial.print(elapsed / 1000.0, 1);
    Serial.println("s");
  }
  else
  {
    stopAllMotors();
    finished = true;
    Serial.println("Stop.");
  }

  delay(100);
}
