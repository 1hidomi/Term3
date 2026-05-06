const int ldrPin = A0;   
const int buzzerPin = 9; 
const int threshold = 600; 

void setup() {
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  int lightValue = analogRead(ldrPin);
  Serial.print("Current Light Level: ");
  Serial.println(lightValue);
  if (lightValue > threshold) {
    tone(buzzerPin, 1000); 
  } else {
    noTone(buzzerPin); 
  }

  delay(100); 
}