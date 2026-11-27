// Onboard NeoPixel output for the Ambient Display.
// Builds on Roopa Ramanujam's tutorial idea (color from temperature, rhythm from wind),
// reworked to be calm: a smooth cool-to-warm gradient and a gentle breathing pulse.
//   waiting (no data yet): dim warm white, slow breath
//   fresh data:            temperature color, breath speed follows wind
//   stale data:            same color, washed out and dimmer, slow breath
// All timing is on millis(); nothing here blocks.
#pragma once
#include <Arduino.h>

void displayBegin();

// Call every loop. temp01 and wind01 are 0..1 and ignored until hasValue is true.
void displayUpdate(uint32_t nowMs, bool hasValue, float temp01, float wind01, bool stale);
