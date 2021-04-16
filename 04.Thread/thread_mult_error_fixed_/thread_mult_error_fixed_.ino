#include <WiFi.h>
#include <FirebaseESP32.h>

TaskHandle_t Task1;
TaskHandle_t Task2;

#define WIFI_SSID "JYGLAB_2.4G"
#define WIFI_PASSWORD "18000658"
#define FIREBASE_HOST "https://ygfirebasetest-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "QKxCxRK3tbuKgXFwzbIddadtjx61L3BaqCgKuDok"

FirebaseData firebaseData1;
FirebaseData firebaseData2;

FirebaseJson json1;
FirebaseJson json2;


void setup() {
  Serial.begin(115200);
  Serial.print("setup is running on core");
  Serial.println(xPortGetCoreID());
  setup_wifi();
  setup_firebase();
  rx();
  
}
void setup_wifi() {
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
  delay(100);
}

void setup_firebase() {
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setReadTimeout(firebaseData1, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData1, "tiny");
  Firebase.setReadTimeout(firebaseData2, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData2, "tiny");
  
}


void rx() {
  xTaskCreatePinnedToCore(
    mainThread,   // 태스크 함수
    "Task1", // 태스크 이름
    8192,  // 스택 크기 (워드 단위)
    NULL,   // 태스크 파라미터
    0,      // 태스크 우선순위
    &Task1,  // 태스크 핸들
    0       // 실행될 코어 
    ); 
//  xTaskCreatePinnedToCore(
//    asyncThread,   // 태스크 함수
//    "Task2", // 태스크 이름
//    10000,  // 스택 크기 (워드 단위)
//    NULL,   // 태스크 파라미터
//    1,      // 태스크 우선순위
//    &Task2,  // 태스크 핸들
//    1       // 실행될 코어 
//    );     
}

void mainThread (void *param) {
  Serial.print("# Task1  running on core :");
  Serial.println(xPortGetCoreID());
  int i = 0;
  while(1) {
    json1.set("/count", i);
    Firebase.updateNode(firebaseData1, "/threadtest/main",json1);
    delay(1000);
    i++;
  }
}

//void asyncThread (void *param) {
//  Serial.print("# Task2  running on core :");
//  Serial.println(xPortGetCoreID());
//  int i = 0;
//  while(1) {
//    json2.set("/count",i);
//    Firebase.updateNode(firebaseData2, "/threadtest/async", json2);
//    i++;
//    delay(2000);
//  }
//  
//}


int j = 0;
void loop() {

  Serial.print("# Task2  running on core :");
  Serial.println(xPortGetCoreID());
  json2.set("/count", j);
  Firebase.updateNode(firebaseData2, "/threadtest/async",json2);
  delay(2000);
  j++;
}
