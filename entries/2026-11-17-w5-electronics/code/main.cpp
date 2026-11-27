// Ambient Display, phase 0: live Berkeley weather on the Feather V2's onboard NeoPixel.
// Port of Roopa Ramanujam's ESP32 web API tutorial (github.com/roopa-ramanujam/ESP32-web-api-example),
// split into net (Wi-Fi, HTTP), data (parse, map to 0..1) and display (NeoPixel).
// Color = temperature (cool blue to warm orange), breathing speed = wind.
#include <Arduino.h>
#include "net.h"
#include "data.h"
#include "display.h"

void setup() {
  Serial.begin(115200);
  Serial.println("\n[AD] Ambient Display phase 0: Open-Meteo weather at Jacobs Hall");
  displayBegin();  // light comes up first, in the waiting state
  netBegin();
}

void loop() {
  uint32_t now = millis();
  netUpdate(now);

  int httpCode;
  String body;
  if (netTakeResult(httpCode, body)) {
    // dataParse marks the failure itself when the body is bad, so only mark here for bad HTTP.
    if (httpCode != 200) dataMarkFailed();
    else dataParse(body, now);
  }

  displayUpdate(now, dataHasValue(), dataTemp01(), dataWind01(), dataIsStale(now));
}
