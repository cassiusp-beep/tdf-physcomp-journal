// Weather data. See data.h. Fields follow Roopa Ramanujam's tutorial (current_weather.temperature
// and current_weather.windspeed), parsed with ArduinoJson instead of Arduino_JSON.
#include "data.h"
#include <ArduinoJson.h>

namespace {

// Berkeley range: about 40 F on a cold winter morning to 85 F on a hot day.
const float kTempColdF = 40.0f;
const float kTempWarmF = 85.0f;
const float kWindCalmMph = 0.0f;
const float kWindStrongMph = 30.0f;  // same top end as the tutorial's blink mapping
const uint32_t kStaleAfterMs = 180000;  // about 3 missed refreshes

bool hasValue = false;
bool lastFetchFailed = false;
float tempF = 0.0f;
float windMph = 0.0f;
uint32_t lastGoodMs = 0;

float map01(float v, float lo, float hi) {
  return constrain((v - lo) / (hi - lo), 0.0f, 1.0f);
}

}  // namespace

bool dataParse(const String& body, uint32_t nowMs) {
  // Keep only the two fields we use, so the document stays small.
  JsonDocument filter;
  filter["current_weather"]["temperature"] = true;
  filter["current_weather"]["windspeed"] = true;

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, body, DeserializationOption::Filter(filter));
  JsonVariant temp = doc["current_weather"]["temperature"];
  JsonVariant wind = doc["current_weather"]["windspeed"];
  if (err || !temp.is<float>() || !wind.is<float>()) {
    Serial.printf("[AD] parse failed: %s\n", err ? err.c_str() : "fields missing");
    dataMarkFailed();
    return false;
  }

  tempF = temp.as<float>();
  windMph = wind.as<float>();
  lastGoodMs = nowMs;
  hasValue = true;
  lastFetchFailed = false;
  Serial.printf("[AD] temp %.1f F, wind %.1f mph -> temp01 %.2f, wind01 %.2f\n",
                tempF, windMph, dataTemp01(), dataWind01());
  return true;
}

void dataMarkFailed() {
  lastFetchFailed = true;
  if (hasValue) {
    Serial.printf("[AD] keeping last good values (%.1f F, %.1f mph)\n", tempF, windMph);
  }
}

bool dataHasValue() { return hasValue; }
float dataTemp01() { return map01(tempF, kTempColdF, kTempWarmF); }
float dataWind01() { return map01(windMph, kWindCalmMph, kWindStrongMph); }

bool dataIsStale(uint32_t nowMs) {
  return hasValue && (lastFetchFailed || nowMs - lastGoodMs > kStaleAfterMs);
}
