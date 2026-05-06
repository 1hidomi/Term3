#include <Wire.h>
#include <MFRC522_I2C.h>

MFRC522_I2C mfrc522(0x28, 9, &Wire); 

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mfrc522.PCD_Init();
  Serial.println("Scanning your card now...");
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()) {
      Serial.print("Found ID:");
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
      }
      Serial.println();
      
      mfrc522.PICC_HaltA();
    }
  }
  delay(100);
}