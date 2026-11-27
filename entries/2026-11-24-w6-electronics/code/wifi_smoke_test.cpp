// Smoke test: proves the driver, toolchain and board work before any real code.
// Expected: the onboard NeoPixel cycles red, green, blue, and Serial prints "tick n" each second.
// Each tick also prints the Wi-Fi MAC address (to register at getonline.berkeley.edu) and whether
// the board has joined the Wi-Fi network in secrets.h, with its IP address once connected.
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include "secrets.h"

Adafruit_NeoPixel pixel(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  pinMode(NEOPIXEL_I2C_POWER, OUTPUT);   // the onboard NeoPixel has its own power pin on the V2
  digitalWrite(NEOPIXEL_I2C_POWER, HIGH);
  pixel.begin();
  pixel.setBrightness(30);
  WiFi.mode(WIFI_STA);   // the station MAC is the one Berkeley IoT registers
  WiFi.begin(WIFI_SSID, WIFI_PASS);   // connects in the background; loop keeps running
}

void loop() {
  static uint32_t n = 0;
  const uint32_t colors[] = {pixel.Color(255, 0, 0), pixel.Color(0, 255, 0), pixel.Color(0, 0, 255)};
  pixel.setPixelColor(0, colors[n % 3]);
  pixel.show();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("tick %lu  MAC %s  Wi-Fi connected, IP %s, signal %d dBm\n", (unsigned long)n++,
                  WiFi.macAddress().c_str(), WiFi.localIP().toString().c_str(), WiFi.RSSI());
  } else {
    Serial.printf("tick %lu  MAC %s  Wi-Fi not connected (status %d)\n", (unsigned long)n++,
                  WiFi.macAddress().c_str(), WiFi.status());
  }
  delay(1000);
}
