#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

typedef struct struct_message {
  float temp;
  float hum;
  float pres;
} struct_message;

volatile struct_message latest;   // updated in callback
volatile bool hasPacket = false;

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  if (len == sizeof(struct_message)) {
    memcpy((void*)&latest, incomingData, sizeof(struct_message));
    hasPacket = true;
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  Serial.print("Receiver MAC: ");
  Serial.println(WiFi.macAddress());

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 failed");
    while (1) {}
  }

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  if (hasPacket) {
    // copy volatile to local (avoid partial reads)
    struct_message msg;
    msg.temp = latest.temp;
    msg.hum  = latest.hum;
    msg.pres = latest.pres;
    hasPacket = false;

    Serial.println("RECEIVED:");
    Serial.print("T: "); Serial.println(msg.temp);
    Serial.print("H: "); Serial.println(msg.hum);
    Serial.print("P: "); Serial.println(msg.pres);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("RECEIVED READINGS");
    display.setCursor(0, 15);
    display.print("Temp: "); display.print(msg.temp); display.print(" C");
    display.setCursor(0, 25);
    display.print("Hum:  "); display.print(msg.hum); display.print(" %");
    display.setCursor(0, 35);
    display.print("Pres: "); display.print(msg.pres); display.print(" hPa");
    display.display();
  }
}
