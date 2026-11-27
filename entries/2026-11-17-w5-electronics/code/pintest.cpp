// Solder joint tester for the Feather ESP32 V2 headers.
// Build and flash with: pio run -e pintest -t upload
//
// Each header pin is tested by touching it with a jumper wire:
//   GND group: hold one end of the wire on a GND pin, touch the other end to the pin.
//   3V group:  hold one end of the wire on the 3V pin, touch the other end to the pin.
// A pin passes when its new state is seen for 3 reads in a row. The NeoPixel flashes
// green and Serial prints "PASS". Every 10 s it prints what is still untested.
// NEVER touch any pin to USB or BAT: the ESP32 is not 5 V tolerant.
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

enum Method { TO_GND, TO_3V_ADC, TO_3V_DIGITAL };

struct Pin {
  const char *label;
  uint8_t gpio;
  Method method;
  uint8_t streak;
  bool passed;
};

Pin pins[] = {
  // Left header (USB end first)
  {"A0 (26)", A0, TO_GND, 0, false},
  {"A1 (25)", A1, TO_GND, 0, false},
  {"A2 (34)", A2, TO_3V_ADC, 0, false},   // input-only, no internal pull
  {"A3 (39)", A3, TO_3V_ADC, 0, false},   // input-only, no internal pull
  {"A4 (36)", A4, TO_3V_ADC, 0, false},   // input-only, no internal pull
  {"A5 (4)", A5, TO_GND, 0, false},
  {"SCK (5)", SCK, TO_GND, 0, false},
  {"MO (19)", MOSI, TO_GND, 0, false},
  {"MI (21)", MISO, TO_GND, 0, false},
  {"RX (7)", RX, TO_GND, 0, false},
  {"TX (8)", TX, TO_GND, 0, false},
  {"37", 37, TO_3V_ADC, 0, false},        // input-only, no internal pull
  // Right header
  {"13", 13, TO_3V_DIGITAL, 0, false},    // red LED pulls it low, so test it high
  {"12", 12, TO_GND, 0, false},
  {"27", 27, TO_GND, 0, false},
  {"33", 33, TO_GND, 0, false},
  {"15", 15, TO_GND, 0, false},
  {"32", 32, TO_GND, 0, false},
  {"14", 14, TO_GND, 0, false},
  {"SCL (20)", SCL, TO_GND, 0, false},
  {"SDA (22)", SDA, TO_GND, 0, false},
};
const size_t PIN_COUNT = sizeof(pins) / sizeof(pins[0]);

Adafruit_NeoPixel pixel(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
uint32_t flashUntil = 0;
uint32_t lastReport = 0;

bool touched(const Pin &p) {
  switch (p.method) {
    case TO_GND:        return digitalRead(p.gpio) == LOW;
    case TO_3V_ADC:     return analogRead(p.gpio) > 3800;   // 12-bit, 3V reads near 4095
    case TO_3V_DIGITAL: return digitalRead(p.gpio) == HIGH;
  }
  return false;
}

void report() {
  size_t left = 0;
  for (size_t i = 0; i < PIN_COUNT; i++) if (!pins[i].passed) left++;
  if (left == 0) {
    Serial.println("ALL PINS PASS. Every header joint tested is good.");
    return;
  }
  Serial.printf("Still to test (%u):\n", (unsigned)left);
  for (size_t i = 0; i < PIN_COUNT; i++) {
    if (pins[i].passed) continue;
    Serial.printf("  %-9s touch to %s\n", pins[i].label, pins[i].method == TO_GND ? "GND" : "3V");
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(NEOPIXEL_I2C_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_I2C_POWER, HIGH);
  pixel.begin();
  pixel.setBrightness(30);
  for (size_t i = 0; i < PIN_COUNT; i++) {
    pinMode(pins[i].gpio, pins[i].method == TO_GND ? INPUT_PULLUP : INPUT);
  }
  delay(50);
  // A pin already reading "touched" with nothing connected is a short or a bridge.
  for (size_t i = 0; i < PIN_COUNT; i++) {
    if (touched(pins[i])) {
      Serial.printf("WARNING: %s reads touched with no wire. Check for a solder bridge.\n", pins[i].label);
    }
  }
  Serial.println("Pin tester ready. GND group: wire from GND. 3V group: wire from 3V.");
  report();
}

void loop() {
  uint32_t now = millis();
  for (size_t i = 0; i < PIN_COUNT; i++) {
    Pin &p = pins[i];
    if (p.passed) continue;
    p.streak = touched(p) ? p.streak + 1 : 0;
    if (p.streak >= 3) {
      p.passed = true;
      Serial.printf("PASS %s\n", p.label);
      flashUntil = now + 300;
    }
  }
  pixel.setPixelColor(0, now < flashUntil ? pixel.Color(0, 255, 0) : pixel.Color(0, 0, 40));
  pixel.show();
  if (now - lastReport > 10000) {
    lastReport = now;
    report();
  }
  delay(20);
}
