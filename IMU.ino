#include <Wire.h>
#include <ICM20948_WE.h>

ICM20948 imu;

void setup() {
  Serial.begin(9600); 
  Wire.begin();
  Serial.println("Initializing ICM20948...");
  if (!imu.begin(Wire, 0x68)) { 
    Serial.println("Not detected");
    while (1);
  }
}

void loop() {
  imu.readSensor();
  Serial.print("Accelerometer: ");
  Serial.print(imu.accelX()); Serial.print(", ");
  Serial.print(imu.accelY()); Serial.print(", ");
  Serial.print(imu.accelZ()); 
  Serial.print(" | Gyroscope: ");
  Serial.print(imu.gyroX()); Serial.print(", ");
  Serial.print(imu.gyroY()); Serial.print(", ");
  Serial.print(imu.gyroZ());
  Serial.println();
  
  delay(100);
}