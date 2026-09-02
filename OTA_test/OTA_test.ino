#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>

const char* ssid = "Jazzy";
const char* password = "123321bb";

// URL ไปยังไฟล์ version.txt และ firmware.bin บน GitHub (raw หรือ release asset)
const char* versionURL  = "https://raw.githubusercontent.com/jazzy6344/OTA_test/refs/heads/main/version.txt";
const char* firmwareURL = "https://raw.githubusercontent.com/jazzy6344/OTA_test/refs/heads/main/firmware.bin";

#define CURRENT_VERSION "1.0.2"

// หมายเหตุ: ใช้ setInsecure() เพื่อข้ามการเช็ค certificate สำหรับทดสอบก่อน
// (Root CA ของ GitHub เปลี่ยนได้เรื่อยๆ ถ้า hardcode ผิดจะต่อไม่ติดเลย)

void checkForUpdate() {
  WiFiClientSecure client;
  client.setInsecure(); // ทดสอบก่อน — ยังไม่เช็ค certificate

  HTTPClient http;
  http.begin(client, versionURL);
  int httpCode = http.GET();

  if (httpCode == 200) {
    String newVersion = http.getString();
    newVersion.trim();

    if (newVersion != CURRENT_VERSION) {
      Serial.printf("พบเวอร์ชันใหม่: %s (ปัจจุบัน %s)\n", newVersion.c_str(), CURRENT_VERSION);

      WiFiClientSecure otaClient;
      otaClient.setInsecure(); // ทดสอบก่อน — ยังไม่เช็ค certificate

      t_httpUpdate_return ret = httpUpdate.update(otaClient, firmwareURL);

      switch (ret) {
        case HTTP_UPDATE_FAILED:
          Serial.printf("อัปเดตล้มเหลว: %s\n", httpUpdate.getLastErrorString().c_str());
          break;
        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("ไม่มีอัปเดต");
          break;
        case HTTP_UPDATE_OK:
          Serial.println("อัปเดตสำเร็จ กำลังรีบูต...");
          break;
      }
    } else {
      Serial.println("เฟิร์มแวร์เป็นเวอร์ชันล่าสุดแล้ว");
    }
  } else {
    Serial.printf("เช็คเวอร์ชันไม่สำเร็จ, HTTP code: %d\n", httpCode);
  }
  http.end();
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  checkForUpdate();
  pinMode(4,OUTPUT);
}

void loop() {
 digitalWrite(4,HIGH);
 
}
