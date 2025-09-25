#include <esp_now.h>
#include <WiFi.h>

#define IBUS_TX_PIN 17
#define IBUS_SERIAL Serial1
#define CHANNELS 14

uint16_t channels[CHANNELS] = {
  1500,1500,1500,1500,
  1000,1000,1000,1000,
  1500,1500,1500,1500,
  1500,1500
};

typedef struct struct_data {
  int16_t joy1X;
  int16_t joy1Y;
  int16_t joy2X;
  int16_t joy2Y;
  uint8_t aux1;
  uint8_t aux2;
} struct_data;

struct_data receivedData;

// AUX translation: 0=1500,1=1300,2=1700
uint16_t auxValue(uint8_t pos) {
  switch(pos){
    case 1: return 1300;
    case 2: return 1700;
    default: return 1500;
  }
}

// Build & send iBUS frame
void sendIBUS(){
  uint8_t packet[32];
  packet[0] = 0x20; packet[1] = 0x40;
  for(int i=0;i<CHANNELS;i++){
    packet[2+i*2] = channels[i] & 0xFF;
    packet[2+i*2+1] = (channels[i] >> 8) & 0xFF;
  }
  uint16_t checksum = 0xFFFF;
  for(int i=0;i<30;i++) checksum -= packet[i];
  packet[30] = checksum & 0xFF;
  packet[31] = checksum >> 8;
  IBUS_SERIAL.write(packet,32);
}

void onDataRecv(const esp_now_recv_info_t *info,const uint8_t *data,int len){
  if(len!=sizeof(receivedData)) return;
  memcpy(&receivedData,data,sizeof(receivedData));

  // Direct mapping (no smoothing, no deadzone)
  channels[0] = map(receivedData.joy1X,0,4095,1000,2000);  // Roll
  channels[1] = map(receivedData.joy1Y,0,4095,1000,2000);  // Pitch
  channels[2] = map(receivedData.joy2X,0,4095,1000,2000);  // Yaw
  channels[3] = map(receivedData.joy2Y,0,4095,1000,2000);  // Throttle
  channels[4] = auxValue(receivedData.aux1);
  channels[5] = auxValue(receivedData.aux2);

  // Debug
  Serial.print("RX -> ");
  Serial.print("J1X: "); Serial.print(channels[0]);
  Serial.print(" J1Y: "); Serial.print(channels[1]);
  Serial.print(" J2X: "); Serial.print(channels[2]);
  Serial.print(" J2Y: "); Serial.print(channels[3]);
  Serial.print(" Aux1: "); Serial.print(channels[4]);
  Serial.print(" Aux2: "); Serial.println(channels[5]);

  sendIBUS();
}

void setup(){
  Serial.begin(115200);
  IBUS_SERIAL.begin(115200,SERIAL_8N1,-1,IBUS_TX_PIN);

  WiFi.mode(WIFI_STA);
  if(esp_now_init()!=ESP_OK){
    Serial.println("ESP-NOW failed");
    while(1);
  }
  esp_now_register_recv_cb(onDataRecv);

  Serial.println("RX ready");
}

void loop(){}
