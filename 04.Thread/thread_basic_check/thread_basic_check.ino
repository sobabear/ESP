#include <WiFi.h>
#include <FirebaseESP32.h>


void setup() {
  Serial.begin(115200);
  Serial.print("setup is running on core");
  Serial.println(xPortGetCoreID());
}

void loop() {
  Serial.print(" * loop() is running on core");
  Serial.println(xPortGetCoreID());
  delay(500);
}
