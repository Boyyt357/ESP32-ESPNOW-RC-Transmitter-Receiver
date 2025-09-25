#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);       // Set Wi-Fi to Station mode
  delay(100);
  Serial.print("RX MAC: ");
  Serial.println(WiFi.macAddress()); // This prints the actual MAC
}

void loop() {}
