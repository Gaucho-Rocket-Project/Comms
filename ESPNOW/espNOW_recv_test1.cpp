// This is the first test of ESPNOW.
// Sending a ping and receiving data

#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>

// put RECEIVER MAC here
uint8_t receiverAddress[] = {0x88, 0x57, 0x21, 0xb1, 0xbb, 0xcc};

typedef struct struct_message
{
  char test[32];
} struct_message;

struct imu_data
{
  int16_t acceleration[3];
  float euler_angles[3];
  uint8_t pressure[3];
  uint8_t temperature[3];
} data;

struct_message TestMessage;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  if (memcmp(mac, receiverAddress, 6) == 0)
  {
    memcpy(&data, incomingData, sizeof(data));
    Serial.print("acceleration[0]: ");
    Serial.println(data.acceleration[0]);
  }
}

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  esp_now_register_recv_cb(OnDataRecv);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }

  strncpy(TestMessage.test, "This is a test", sizeof(TestMessage.test));
  TestMessage.test[sizeof(TestMessage.test) - 1] = '\0';
}

void loop()
{

  esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)&TestMessage, sizeof(TestMessage));
  Serial.println(result == ESP_OK ? "Sent packet" : "Send error");

  delay(10000);
}