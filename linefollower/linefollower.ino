#include "Arduino.h"
#define PWMA 23
#define AIN2 22
#define AIN1 21

#define BIN1 19
#define BIN2 18
#define PWMB 5

#define STBY 17

#define RIR 13
#define LIR 27
#define SIR 35 // detect station

#define FWSPD 100
#define BWSPD -75

void setup() {
 pinMode(PWMA, OUTPUT);
 pinMode(AIN2, OUTPUT);
 pinMode(AIN1, OUTPUT);
 pinMode(PWMB, OUTPUT);
 pinMode(BIN1, OUTPUT);
 pinMode(BIN2, OUTPUT);
 pinMode(STBY, OUTPUT);
 pinMode(RIR, INPUT);
 pinMode(LIR, INPUT);
 pinMode(SIR, INPUT);
 Serial.begin(9600);
}

void mv(int lspeed, int rspeed) {
  digitalWrite(STBY, HIGH);

  if (lspeed >= 0) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    analogWrite(PWMA, lspeed);
  } else {
    digitalWrite(AIN2, HIGH);
    digitalWrite(AIN1, LOW);
    analogWrite(PWMA, -lspeed);
  }

  if (rspeed >= 0) {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    analogWrite(PWMB, rspeed);
  } else {
    digitalWrite(BIN2, HIGH);
    digitalWrite(BIN1, LOW);
    analogWrite(PWMB, -rspeed);
  }
}

void line() {
  int l = digitalRead(LIR);
  int r = digitalRead(RIR);
  if (l == LOW && r == LOW) {
    mv(FWSPD, FWSPD);
  } else if (l == HIGH && r == LOW) {
    // too far left, turn right
    mv(BWSPD, FWSPD);
  } else if (l == LOW && r == HIGH) {
    // too far right, turn left
    mv(FWSPD, BWSPD);
  }
}

void loop() {
  // if SIR gives HIGH, check after 0.5s to see if it's still HIGH
  // if yes, then reached station
  if (digitalRead(SIR) == HIGH) {
    unsigned long start = millis();
    double avg = 1;
    while (millis() - start < 200) {
      line();
      // the problem is we're getting random LOW readings
      // gotta take avg of prev readings
      avg += digitalRead(SIR);
      avg /= 2;
      if (avg < 0.2) {
        break;
      }
    }
    if (avg > 0.5) {
      mv(0, 0);
      delay(random(1000,5000));
    }
  }
  line();
}
