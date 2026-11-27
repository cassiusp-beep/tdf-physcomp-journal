// Onboard NeoPixel. See display.h. Temperature color and wind rhythm follow Roopa Ramanujam's
// tutorial, with her three color bands and delay() blink replaced by a gradient and a millis() breath.
#include "display.h"
#include <Adafruit_NeoPixel.h>

namespace {

Adafruit_NeoPixel pixel(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

const float kMaxLevel = 60.0f / 255.0f;  // modest peak brightness for a bare LED on a desk
const uint32_t kFrameMs = 20;            // about 50 frames per second

// Breath period: slow when calm, quicker when windy (Roopa mapped 0..30 mph to 800..100 ms blinks).
const float kBreathCalmMs = 6000.0f;
const float kBreathWindyMs = 1500.0f;
const float kBreathFloor = 0.45f;        // never dimmer than this share of the peak while fresh

// Waiting for the first fetch: dim warm white.
const float kWaitColor[3] = {255, 170, 110};
const float kWaitLevel = 0.35f;
const float kWaitBreathMs = 4000.0f;

// Stale data: washed toward grey, dimmer, slow breath regardless of wind.
const float kStaleDesaturate = 0.6f;
const float kStaleLevel = 0.45f;
const float kStaleBreathMs = 8000.0f;

// Cool-to-warm gradient stops, evenly spaced from 0 (cold) to 1 (warm).
const float kStops[][3] = {
    {30, 70, 255},    // cool blue
    {20, 170, 210},   // teal
    {255, 200, 90},   // soft gold
    {255, 95, 15},    // warm orange
};
const int kStopCount = sizeof(kStops) / sizeof(kStops[0]);

float phase = 0.0f;  // breath phase in cycles; advanced by elapsed time so speed changes stay smooth
uint32_t lastFrameMs = 0;

void gradient(float t, float out[3]) {
  float x = constrain(t, 0.0f, 1.0f) * (kStopCount - 1);
  int i = min((int)x, kStopCount - 2);
  float f = x - i;
  for (int c = 0; c < 3; c++) out[c] = kStops[i][c] + (kStops[i + 1][c] - kStops[i][c]) * f;
}

// 0..1 breath, smooth at both ends.
float breath(float floorLevel) {
  float wave = 0.5f - 0.5f * cosf(phase * TWO_PI);
  return floorLevel + (1.0f - floorLevel) * wave;
}

void show(const float rgb[3], float level) {
  float s = level * kMaxLevel;
  pixel.setPixelColor(0, pixel.Color((uint8_t)(rgb[0] * s), (uint8_t)(rgb[1] * s), (uint8_t)(rgb[2] * s)));
  pixel.show();
}

}  // namespace

void displayBegin() {
  pinMode(NEOPIXEL_I2C_POWER, OUTPUT);  // the V2's onboard NeoPixel has its own power pin
  digitalWrite(NEOPIXEL_I2C_POWER, HIGH);
  pixel.begin();
  pixel.setBrightness(255);  // brightness is scaled in show() instead, which keeps more color detail
  show(kWaitColor, kWaitLevel * kBreathFloor);
}

void displayUpdate(uint32_t nowMs, bool hasValue, float temp01, float wind01, bool stale) {
  uint32_t dt = nowMs - lastFrameMs;
  if (dt < kFrameMs) return;
  lastFrameMs = nowMs;

  float rgb[3];
  float periodMs;
  float level;

  if (!hasValue) {
    periodMs = kWaitBreathMs;
    for (int c = 0; c < 3; c++) rgb[c] = kWaitColor[c];
    level = kWaitLevel;
  } else {
    gradient(temp01, rgb);
    if (stale) {
      float grey = (rgb[0] + rgb[1] + rgb[2]) / 3.0f;
      for (int c = 0; c < 3; c++) rgb[c] += (grey - rgb[c]) * kStaleDesaturate;
      periodMs = kStaleBreathMs;
      level = kStaleLevel;
    } else {
      periodMs = kBreathCalmMs + (kBreathWindyMs - kBreathCalmMs) * constrain(wind01, 0.0f, 1.0f);
      level = 1.0f;
    }
  }

  phase += dt / periodMs;
  phase -= floorf(phase);
  show(rgb, level * breath(kBreathFloor));
}
