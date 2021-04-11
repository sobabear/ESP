#include <WiFi.h>
#include <FirebaseESP32.h>

#define WIFI_SSID "JYGLAB_2.4G"
#define WIFI_PASSWORD "18000658"

#define FIREBASE_HOST "https://ygfirebasetest-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "QKxCxRK3tbuKgXFwzbIddadtjx61L3BaqCgKuDok"

FirebaseData firebaseData;
FirebaseAuth mAuth;
FirebaseJson json;
FirebaseConfig mConfig;

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
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
  Firebase.reconnectWiFi(true);
    //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
  /*
  This option allows get and delete functions (PUT and DELETE HTTP requests) works for device connected behind the
  Firewall that allows only GET and POST requests.
  
  Firebase.enableClassicRequest(firebaseData, true);
  */

  //String path = "/data";
  
  Serial.println("------------------------------------");
  Serial.println("Connected...");
}

void loop() {
  // put your main code here, to run repeatedly:
  json.set("/esp", i);
  i++;
  Firebase.updateNode(firebaseData,"/wow",json);
  delay(500);

}
