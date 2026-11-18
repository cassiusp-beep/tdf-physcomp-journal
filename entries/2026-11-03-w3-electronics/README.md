---
title: W3 E1: Motion, detected: a PIR sensor that moves a servo
date: 2026-11-03
week: 3
type: electronics
project: pre-projects
slides: C19
medium:
status: draft
---

# W3 E1: Motion, detected: a PIR sensor that moves a servo

**Week 3 · Electronics · Tue Nov 3, 2026**
Lecture: [C19](../../sources/lecture-slides.md) · Medium article: _add link_

## Assignment

"Make 'something interesting' happen with: servo motor; ultrasonic or PIR sensor. Document this work in your design journal" [C17, slide 28].

## What I did

I wired a PIR motion sensor to a servo so that a person walking into view makes the machine physically react. I built and demoed it in class on Tuesday, November 3.

I picked the PIR because it answers a more human question. A distance sensor asks *how far away are you?* A PIR asks *is anyone here?*

**Circuit.** Three kit parts: Elegoo Arduino Uno R3, PIR motion sensor, micro servo, all on the Arduino's 5V rail. Prototyped in Tinkercad first, then built on a breadboard. Red jumpers carry 5V and black carry ground, per the class color code [C17, slide 21].

| Part | Pin | Arduino |
|---|---|---|
| PIR sensor | VCC | 5V |
| PIR sensor | GND | GND |
| PIR sensor | OUT | Digital pin 2 |
| Micro servo | Red | 5V |
| Micro servo | Brown | GND |
| Micro servo | Orange (signal) | Digital pin 9 |

**Code.** A loop that asks one question: is the PIR's output HIGH? On motion, the servo turns to its alert angle; when motion stops, it returns to rest. The sketch prints the sensor state to the Serial Monitor. A PIR only outputs on or off and holds HIGH for a few seconds after motion stops, so the servo can't track *where* you are, only *that* you are.

**Documentation note.** I didn't capture photos or video of the physical build in class, and it has since been taken apart. The Tinkercad model is the design the hardware was built from, and the sketch below implements the same behavior.

## Media

<!-- Tinkercad breadboard screenshot, schematic export, screen recording of the simulation:
![Tinkercad breadboard](media/tinkercad-breadboard.png)
![Tinkercad schematic](media/tinkercad-schematic.png)
[Video: simulation](media/pir-servo-sim.mp4) -->

## Code and circuit

Full sketch: [`code/pir_servo.ino`](code/pir_servo.ino). The wiring table above is the circuit; add the Tinkercad schematic export to `media/` for the diagram.

```cpp
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
```

## Results and reflection

The PIR worked as built: walk into its field and the servo turns to its alert angle, step away and it settles back to rest. Choosing a PIR over the ultrasonic sensor changed what the machine could know. A distance sensor asks how far away you are. A PIR only asks whether anyone is there, so the servo can switch between two states but can never track you.

The PIR's hold time turned into behavior. Because the output stays HIGH for a few seconds after motion stops, the servo lingers before it relaxes. In a character, that pause could read as attention or hesitation, which is a useful idea going into Expressive Mechanics.

Prototyping in Tinkercad first caught wiring and logic mistakes before I touched the kit. The lesson I'm keeping is about documentation: I demoed this in class without filming it, and the build is gone. Next time the phone comes out before the breadboard does.

_AI use: drafted with Claude from my build notes, README and Medium posts, then reviewed by me [C01, slide 46]._

## Sources

- [C19] 2026 TDF - PhysComp class 19 - Project 1 intro - p5.js
- [C17] class 17 deck, servo + sensor homework slide and wire color codes
