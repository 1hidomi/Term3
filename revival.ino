const int btnPin = 13;
const int redPin = 14;
const int greenPin = 15;
bool isRevived = false;    
bool lastBtnState = HIGH;   

void setup() {
  pinMode(btnPin, INPUT_PULLUP);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, LOW);
}

void loop() {
  bool currentBtnState = digitalRead(btnPin);
  if (lastBtnState == HIGH && currentBtnState == LOW) {
    delay(50); 
    if (digitalRead(btnPin) == LOW) {
      isRevived = !isRevived;
    }
  }
  if (isRevived) {
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, HIGH);
  } else {
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, LOW);
  }
  lastBtnState = currentBtnState;
}
