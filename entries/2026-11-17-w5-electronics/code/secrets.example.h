// Copy this file to include/secrets.h and fill it in. secrets.h is gitignored: never commit it.
#pragma once
#define WIFI_SSID "your-network"
#define WIFI_PASS "your-password"
// Optional second network for off campus (home Wi-Fi or phone hotspot, 2.4 GHz only).
// Delete these two lines if you only use one network. The board tries network 1 first,
// then alternates every 15 s until one joins.
#define WIFI_SSID_2 "your-home-or-hotspot"
#define WIFI_PASS_2 "your-password"
#define API_KEY   ""   // leave empty if the API needs no key
