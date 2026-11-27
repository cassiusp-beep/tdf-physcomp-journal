// Weather data for the Ambient Display: parse the Open-Meteo reply, keep the last good
// values, and map them to 0..1 for the display.
// Reading temperature and wind from current_weather is the idea in Roopa Ramanujam's
// ESP32 web API tutorial: github.com/roopa-ramanujam/ESP32-web-api-example
#pragma once
#include <Arduino.h>

// Parse a 200 reply. On success, stores temp and wind as the last good values and returns true.
bool dataParse(const String& body, uint32_t nowMs);

// Record a failed fetch (network error, non-200, or bad JSON). Last good values are kept.
void dataMarkFailed();

bool dataHasValue();     // false until the first good fetch
float dataTemp01();      // 0 = cold end, 1 = warm end (clamped)
float dataWind01();      // 0 = calm, 1 = windy (clamped)

// True when the last fetch failed or the last good one is older than about 3 minutes.
bool dataIsStale(uint32_t nowMs);
