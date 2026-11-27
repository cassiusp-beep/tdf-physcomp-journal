// Wi-Fi and HTTP for the Ambient Display.
// The API call (Open-Meteo current weather at Jacobs Hall, plain http) comes from
// Roopa Ramanujam's ESP32 web API tutorial: github.com/roopa-ramanujam/ESP32-web-api-example
//
// Wi-Fi joins in the background and reconnects on its own. If include/secrets.h also defines
// WIFI_SSID_2 / WIFI_PASS_2, it alternates between the two networks every 15 s until one joins. The HTTP GET runs in its own
// FreeRTOS task on a 60 s timer, so a slow or failed request never stalls the display loop.
#pragma once
#include <Arduino.h>

// Start Wi-Fi (credentials from include/secrets.h), print the MAC once, start the fetch task.
void netBegin();

// Call every loop: watches the Wi-Fi link, logs changes and nudges a reconnect (or switches to
// the other network) when it is down.
void netUpdate(uint32_t nowMs);

// True once per finished fetch. httpCode is the HTTP status (200 on success) or a negative
// HTTPClient error. body holds the response text only when httpCode is 200.
bool netTakeResult(int& httpCode, String& body);
