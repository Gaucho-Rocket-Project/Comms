//First test of ESPNOW
//Receieve a ping and send data

#include <WiFi.h>
#include <Wire.h>
#include <esp_now.h>
#include <cmath>
#include <iostream>
#include <utility>
#include <vector>
#include <array>  // added for lookup table

uint8_t ground_station[] = {0x88, 0x57, 0x21, 0xB2, 0x40, 0x8C};


struct imu_data{
  int16_t acceleration[3];
  float euler_angles[3];
  uint8_t pressure[3];
  uint8_t temperature[3];
} data;

int count = 0;

void onDataRecv(const esp_now_recv_info_t *recvInfo, const uint8_t *incomingData, int len){
  if(memcmp(recvInfo->src_addr, ground_station, 6) == 0){
    esp_now_send(recvInfo->src_addr, (uint8_t *)&data, sizeof(data));
    count++;
    data.acceleration[0] = count;
    Serial.print("Received: ");
  } 
}

void onDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  data.acceleration[0] = count;

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, ground_station, sizeof(ground_station));
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  esp_now_register_recv_cb(onDataRecv);
  esp_now_register_send_cb(onDataSent);
}

void loop() {
  // put your main code here, to run repeatedly:

}
