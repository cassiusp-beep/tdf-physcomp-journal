// Wi-Fi and HTTP. See net.h. API call adapted from Roopa Ramanujam's ESP32 web API tutorial.
#include "net.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <esp_mac.h>
#include "secrets.h"

namespace {

// Open-Meteo is keyless. Plain http is what works on Berkeley-IoT (Roopa's note).
// windspeed_unit=mph added so the wind value really is mph (Open-Meteo defaults to km/h).
const char* const kWeatherUrl =
    "http://api.open-meteo.com/v1/forecast?latitude=37.876270&longitude=-122.258499"
    "&current_weather=true&temperature_unit=fahrenheit&windspeed_unit=mph";

const uint32_t kFetchIntervalMs = 60000;     // one request a minute, as in the tutorial
const uint32_t kHttpTimeoutMs = 4000;        // give up on a slow server after 4 s
const uint32_t kReconnectIntervalMs = 15000; // nudge Wi-Fi this often while it is down
const uint32_t kFetchTaskStack = 8192;

// Networks to try, in order. The second pair is optional (off campus: home or phone hotspot).
// WiFi.begin() returns at once, so switching networks never stalls the display loop.
struct WifiNet { const char* ssid; const char* pass; };
const WifiNet kNets[] = {
  {WIFI_SSID, WIFI_PASS},
#ifdef WIFI_SSID_2
  {WIFI_SSID_2, WIFI_PASS_2},
#endif
};
const size_t kNetCount = sizeof(kNets) / sizeof(kNets[0]);
size_t netIndex = 0;  // network currently being tried

void joinNet(size_t i) {
  netIndex = i;
  // Drop the previous attempt first. Without this, auto-reconnect keeps retrying the old
  // network and the switch never takes (seen Nov 27: a hidden home network never joined).
  WiFi.disconnect();
  WiFi.begin(kNets[i].ssid, kNets[i].pass);  // background join; returns immediately
  Serial.printf("[AD] Wi-Fi joining %s...\n", kNets[i].ssid);
}

SemaphoreHandle_t resultLock = nullptr;
bool resultReady = false;
int resultCode = 0;
String resultBody;

// Runs in its own task: waits for Wi-Fi, then fetches right away and every kFetchIntervalMs.
void fetchTask(void*) {
  bool fetchedOnce = false;
  uint32_t lastAttemptMs = 0;
  for (;;) {
    uint32_t now = millis();
    bool due = !fetchedOnce || now - lastAttemptMs >= kFetchIntervalMs;
    if (due && WiFi.status() == WL_CONNECTED) {
      fetchedOnce = true;
      lastAttemptMs = now;

      HTTPClient http;
      http.setConnectTimeout(kHttpTimeoutMs);
      http.setTimeout(kHttpTimeoutMs);
      int code = -1;
      String body;
      if (http.begin(kWeatherUrl)) {
        code = http.GET();
        if (code == HTTP_CODE_OK) body = http.getString();
        http.end();
      }
      uint32_t took = millis() - now;
      if (code == HTTP_CODE_OK) {
        Serial.printf("[AD] fetch: HTTP %d, %u bytes, %lu ms, heap %lu (low %lu)\n", code, body.length(),
                      (unsigned long)took, (unsigned long)ESP.getFreeHeap(), (unsigned long)ESP.getMinFreeHeap());
      } else {
        Serial.printf("[AD] fetch failed: HTTP %d (%s), %lu ms\n", code,
                      code < 0 ? HTTPClient::errorToString(code).c_str() : "server error",
                      (unsigned long)took);
        Serial.printf("[AD] heap %lu (low %lu)\n", (unsigned long)ESP.getFreeHeap(), (unsigned long)ESP.getMinFreeHeap());
      }

      xSemaphoreTake(resultLock, portMAX_DELAY);
      resultCode = code;
      resultBody = body;
      resultReady = true;
      xSemaphoreGive(resultLock);
    }
    vTaskDelay(pdMS_TO_TICKS(250));
  }
}

}  // namespace

void netBegin() {
  resultLock = xSemaphoreCreateMutex();
  // The station MAC is the one Berkeley IoT registers. Read it from eFuse, because
  // WiFi.macAddress() returns zeros until the Wi-Fi driver has fully started.
  uint8_t mac[6];
  esp_read_mac(mac, ESP_MAC_WIFI_STA);
  Serial.printf("[AD] MAC %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  joinNet(0);  // joins in the background; loop keeps running
  xTaskCreatePinnedToCore(fetchTask, "fetch", kFetchTaskStack, nullptr, 1, nullptr, 0);
}

void netUpdate(uint32_t nowMs) {
  static bool wasConnected = false;
  static uint32_t lastNudgeMs = 0;
  bool connected = WiFi.status() == WL_CONNECTED;

  if (connected && !wasConnected) {
    Serial.printf("[AD] Wi-Fi connected to %s, IP %s, signal %d dBm\n", WiFi.SSID().c_str(),
                  WiFi.localIP().toString().c_str(), WiFi.RSSI());
  } else if (!connected && wasConnected) {
    Serial.println("[AD] Wi-Fi lost, reconnecting");
    lastNudgeMs = nowMs;
  }
  wasConnected = connected;

  // Auto-reconnect usually handles a drop; this is a backstop if it stalls.
  // With two networks, each failed 15 s window hands over to the other one.
  if (!connected && nowMs - lastNudgeMs >= kReconnectIntervalMs) {
    lastNudgeMs = nowMs;
    Serial.printf("[AD] Wi-Fi still down on %s (status %d)\n", kNets[netIndex].ssid, WiFi.status());
    if (kNetCount > 1) {
      joinNet((netIndex + 1) % kNetCount);
    } else {
      WiFi.reconnect();
    }
  }
}

bool netTakeResult(int& httpCode, String& body) {
  if (resultLock == nullptr) return false;
  bool got = false;
  xSemaphoreTake(resultLock, portMAX_DELAY);
  if (resultReady) {
    httpCode = resultCode;
    body = resultBody;
    resultBody = String();
    resultReady = false;
    got = true;
  }
  xSemaphoreGive(resultLock);
  return got;
}
