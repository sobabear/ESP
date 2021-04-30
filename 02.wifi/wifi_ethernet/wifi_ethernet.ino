#include <WiFi.h>
#include <FirebaseESP32.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define WIFI_SSID "JYGLAB_2.4G"
#define WIFI_PASSWORD "18000658"

#define FIREBASE_HOST "https://ygfirebasetest-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "QKxCxRK3tbuKgXFwzbIddadtjx61L3BaqCgKuDok"

FirebaseData firebaseData;
FirebaseAuth mAuth;
FirebaseJson json;
FirebaseConfig mConfig;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 32400;
const int   daylightOffset_sec = 3600;

int mYear, mMonth, mDate;
String formattedDate;
String timeStamp;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);


int i = 0;

void setup() {
  Serial.begin(115200); 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting to Wi-fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("conneted with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  timeClient.begin();
  timeClient.setTimeOffset(32400);

  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

  //String path = "/data";
  
  Serial.println("------------------------------------");
  Serial.println("Connected...");
}

void loop() {
  // put your main code here, to run repeatedly:
  timeClient.update();
  timeStamp = timeClient.getFullFormattedTime();
  Serial.println(timeStamp);
 
  Serial.println(timeClient.getFormattedTime());
  json.set("/timestamp", timeStamp);
  json.set("/esp", i);
  i++;
  Firebase.updateNode(firebaseData,"/wow",json);
  delay(500);
  

}
