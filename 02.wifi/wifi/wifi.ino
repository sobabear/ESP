#include <WiFi.h>

const char* ssid = "JYGLAB_5G";
const char* password = "18000658";
int i = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

}


// 연결 여부 로그 출력
void loop() {
  // put your main code here, to run repeatedly:
  if (WiFi.status() == WL_CONNECTED)
    Serial.println("WIFI Connected");
  else
    Serial.println("WIFI not Connected");
  delay(1000);

}
