#include <esp_now.h>
#include <WiFi.h>
#include "esp_wifi.h"   // required for esp_wifi_* functions

// ---------- Joystick pins ----------
#define JOY1_X 34
#define JOY1_Y 35
#define JOY2_X 32
#define JOY2_Y 33

// ---------- Switch pins (3.3V middle wiring) ----------
#define SW1_UP_PIN    25
#define SW1_DOWN_PIN  26
#define SW2_UP_PIN    27
#define SW2_DOWN_PIN  14

// ---------- Deadzone ----------
#define DEADZONE 34

// ---------- Data struct ----------
typedef struct struct_data {
  int16_t joy1X;
  int16_t joy1Y;
  int16_t joy2X;
  int16_t joy2Y;
  uint8_t aux1;
  uint8_t aux2;
} struct_data;

struct_data sendData;

// Calibration storage
int16_t j1x_center, j1y_center, j2x_center, j2y_center;

// Peer MAC (replace with RX MAC from serial monitor)
uint8_t rxMac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Send status callback (new API for ESP32 Arduino 3.x)
void OnDataSent(const esp_now_send_info_t *info, esp_now_send_status_t status) {
  Serial.print("TX -> Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Apply deadzone
int16_t applyDeadzone(int16_t raw, int16_t center) {
  int16_t diff = raw - center;
  if (abs(diff) < DEADZONE) return center;
  return raw;
}

void setup() {
  Serial.begin(115200);

  // Joystick inputs
  pinMode(JOY1_X, INPUT);
  pinMode(JOY1_Y, INPUT);
  pinMode(JOY2_X, INPUT);
  pinMode(JOY2_Y, INPUT);

  // Switch inputs (3.3V center wiring → use INPUT, no pullups)
  pinMode(SW1_UP_PIN, INPUT);
  pinMode(SW1_DOWN_PIN, INPUT);
  pinMode(SW2_UP_PIN, INPUT);
  pinMode(SW2_DOWN_PIN, INPUT);

  // ESP-NOW init
  WiFi.mode(WIFI_STA);

  // Force long-range mode and max TX power (if supported)
  esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_LR);
  esp_wifi_set_max_tx_power(84); // 20 dBm

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, rxMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Auto calibration of joystick centers
  j1x_center = analogRead(JOY1_X);
  j1y_center = analogRead(JOY1_Y);
  j2x_center = analogRead(JOY2_X);
  j2y_center = analogRead(JOY2_Y);

  Serial.println("TX ready with auto-calibration");
}

void loop() {
  // Read joysticks with deadzone
  sendData.joy1X = applyDeadzone(analogRead(JOY1_X), j1x_center);
  sendData.joy1Y = applyDeadzone(analogRead(JOY1_Y), j1y_center);
  sendData.joy2X = applyDeadzone(analogRead(JOY2_X), j2x_center);
  sendData.joy2Y = applyDeadzone(analogRead(JOY2_Y), j2y_center);

  // Switches (3.3V center logic)
  if (digitalRead(SW1_UP_PIN) == HIGH)        sendData.aux1 = 1;  // UP = 1300
  else if (digitalRead(SW1_DOWN_PIN) == HIGH) sendData.aux1 = 2;  // DOWN = 1700
  else                                        sendData.aux1 = 0;  // MID = 1500

  if (digitalRead(SW2_UP_PIN) == HIGH)        sendData.aux2 = 1;
  else if (digitalRead(SW2_DOWN_PIN) == HIGH) sendData.aux2 = 2;
  else                                        sendData.aux2 = 0;

  // Debug print
  Serial.print("TX -> ");
  Serial.print("J1X: "); Serial.print(sendData.joy1X);
  Serial.print(" J1Y: "); Serial.print(sendData.joy1Y);
  Serial.print(" J2X: "); Serial.print(sendData.joy2X);
  Serial.print(" J2Y: "); Serial.print(sendData.joy2Y);
  Serial.print(" Aux1: "); Serial.print(sendData.aux1);
  Serial.print(" Aux2: "); Serial.println(sendData.aux2);

  // Send data
  esp_now_send(rxMac, (uint8_t *)&sendData, sizeof(sendData));

  delay(20); // ~50Hz send rate
}
