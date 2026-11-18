/*
  W3 E1: Motion, detected
  A PIR motion sensor turns a servo to an "alert" angle when someone is
  present, and lets it settle back to rest once the PIR goes quiet.

  Wiring (Arduino Uno R3):
    PIR VCC  -> 5V        PIR GND -> GND        PIR OUT -> pin 2
    Servo red -> 5V       Servo brown -> GND    Servo orange (signal) -> pin 9
*/

#include <Servo.h>

const int PIR_PIN   = 2;
const int SERVO_PIN = 9;

const int REST_ANGLE  = 0;    // nobody here
const int ALERT_ANGLE = 90;   // someone walked in
const int STEP_DELAY  = 8;    // ms per degree, so the turn reads as a gesture

Servo gesture;
int currentAngle = REST_ANGLE;
int lastState = LOW;

void easeTo(int target) {
  while (currentAngle != target) {
    currentAngle += (target > currentAngle) ? 1 : -1;
    gesture.write(currentAngle);
    delay(STEP_DELAY);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(PIR_PIN, INPUT);
  gesture.attach(SERVO_PIN);
  gesture.write(REST_ANGLE);
  Serial.println("Warming up the PIR...");
  delay(2000);   // PIR sensors need a moment to settle after power-up
  Serial.println("Ready");
}

void loop() {
  int state = digitalRead(PIR_PIN);

  if (state != lastState) {          // only act when presence changes
    if (state == HIGH) {
      Serial.println("Motion detected -> alert");
      easeTo(ALERT_ANGLE);
    } else {
      Serial.println("Motion ended -> rest");
      easeTo(REST_ANGLE);
    }
    lastState = state;
  }

  delay(50);
}
