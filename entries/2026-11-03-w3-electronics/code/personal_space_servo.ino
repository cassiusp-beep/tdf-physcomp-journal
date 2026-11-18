#include <Servo.h>

// Personal Space Servo: the servo horn is a sign.
// Nobody close -> sign lies HORIZONTAL (no interaction).
// Someone too close -> sign stands VERTICAL and gives a warning shake.

const int TRIG_PIN = 7;
const int ECHO_PIN = 6;
const int SERVO_PIN = 9;

const int PRESENCE_CM = 50;    // closer than this counts as "too close"
const int HYSTERESIS = 5;      // keeps it from flickering at the edge

const unsigned long ENTER_DELAY_MS = 500;   // must stay close this long before it reacts
const unsigned long EXIT_DELAY_MS = 1000;   // holds the sign up this long after you leave

const int ANGLE_IDLE = 0;        // horizontal
const int ANGLE_TOO_CLOSE = 90;  // vertical
const int SHAKE_AMOUNT = 15;     // degrees each way for the warning shake

Servo sign;
bool someoneThere = false;
int currentAngle = ANGLE_IDLE;
float smoothedCm = 300;
unsigned long closeSince = 0;
unsigned long clearSince = 0;

long readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 400;
  return duration / 58;
}

void easeTo(int target, int stepDelay) {
  while (currentAngle != target) {
    currentAngle += (target > currentAngle) ? 1 : -1;
    sign.write(currentAngle);
    delay(stepDelay);
  }
}

void warningShake() {
  for (int i = 0; i < 2; i++) {
    sign.write(ANGLE_TOO_CLOSE - SHAKE_AMOUNT); delay(80);
    sign.write(ANGLE_TOO_CLOSE + SHAKE_AMOUNT); delay(80);
  }
  sign.write(ANGLE_TOO_CLOSE);
}

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  sign.attach(SERVO_PIN);
  sign.write(ANGLE_IDLE);
}

void loop() {
  long raw = readDistanceCm();
  smoothedCm = smoothedCm * 0.6 + raw * 0.4;
  unsigned long now = millis();

  if (!someoneThere) {
    if (smoothedCm < PRESENCE_CM - HYSTERESIS) {
      if (closeSince == 0) closeSince = now;
      if (now - closeSince >= ENTER_DELAY_MS) {
        someoneThere = true;
        clearSince = 0;
        easeTo(ANGLE_TOO_CLOSE, 5);
        warningShake();
      }
    } else {
      closeSince = 0;   // they didn't stay, so reset the timer
    }
  } else {
    if (smoothedCm > PRESENCE_CM + HYSTERESIS) {
      if (clearSince == 0) clearSince = now;
      if (now - clearSince >= EXIT_DELAY_MS) {
        someoneThere = false;
        closeSince = 0;
        easeTo(ANGLE_IDLE, 5);
      }
    } else {
      clearSince = 0;   // they came back, so keep the sign up
    }
  }

  Serial.print("distance: ");
  Serial.print((int)smoothedCm);
  Serial.println(someoneThere ? " cm   TOO CLOSE (sign vertical)" : " cm   CLEAR (sign horizontal)");
  delay(60);
}
