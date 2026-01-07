#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;

// put RECEIVER MAC here
uint8_t receiverAddress[] = {0x7C, 0x9E, 0xBD, 0x12, 0x34, 0x56};

typedef struct struct_message {
  float temp;
  float hum;
  float pres;
} struct_message;

struct_message BME280Readings;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (!bme.begin(0x76)) {
    Serial.println("BME280 not found");
    while (1) {}
  }

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  BME280Readings.temp = bme.readTemperature();
  BME280Readings.hum  = bme.readHumidity();
  BME280Readings.pres = bme.readPressure() / 100.0F;

  esp_err_t result = esp_now_send(receiverAddress, (uint8_t*)&BME280Readings, sizeof(BME280Readings));
  Serial.println(result == ESP_OK ? "Sent packet" : "Send error");

  delay(10000);
}
