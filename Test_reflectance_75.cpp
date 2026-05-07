#include <Arduino.h>

const int dataPins[] = {10, 11, 12}; 
const int numSensors = 3;

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long duration[3];
  for (int i = 0; i < numSensors; i++) {
    pinMode(dataPins[i], OUTPUT);
    digitalWrite(dataPins[i], HIGH);
    delayMicroseconds(10); 
    pinMode(dataPins[i], INPUT);
    unsigned long startTime = micros();
    while (digitalRead(dataPins[i]) == HIGH && (micros() - startTime) < 3000);
    duration[i] = micros() - startTime;
  }
  Serial.print("Left: "); Serial.print(duration[2]);
  Serial.print(" | ");
  Serial.print("Medium: "); Serial.print(duration[1]);
  Serial.print(" | ");
  Serial.print("Right: "); Serial.println(duration[0]);

  delay(100); 
}
