
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif


#define USER_EMAIL "yookgak@yookgak.com"
#define USER_PASSWORD "000000"
#define FIREBASE_HOST "ygfirebasetest-default-rtdb.firebaseio.com"
#define WIFI_SSID "JYG_LAB_5G"
#define WIFI_PASSWORD "18000658"
#define FIREBASE_SECRET "QKxCxRK3tbuKgXFwzbIddadtjx61L3BaqCgKuDok"
#define FIREBASE_API_KEY "AIzaSyCYvY4C65cp8EToT8ekWVVL0HcHYnT7krE"

FirebaseData FRdata;
FirebaseData firebaseData1;
FirebaseData firebaseData2;

FirebaseAuth auth;
FirebaseConfig config;

void setup()
{

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println(".. progress connecting WIFI");
    delay(300);
  }
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();



  /* 7. Assign the project host and api key (required) */
  config.host = FIREBASE_HOST;
  config.api_key = API_KEY;

  /* 8. Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  
  /* 9. Initialize the library with the autentication data */
  Firebase.begin(&config, &auth);

  /* 10. Enable auto reconnect the WiFi when connection lost */
  Firebase.reconnectWiFi(true);



}

void loop()
{
      if (!Firebase.readStream(firebaseData1))
    {
        Serial.println();
        Serial.println("Can't read stream data");
        Serial.println("REASON: " + firebaseData1.errorReason());
        Serial.println();
    }

    if (firebaseData1.streamTimeout())
    {
        Serial.println();
        Serial.println("Stream timeout, resume streaming...");
        Serial.println();
    }
  if(Firebase.RTDB.setInt(firebaseData1, "/esptest", 1))
  {
    //Success
     Serial.println("Set int data success");

  }else{
    //Failed?, get the error reason from fbdo

    Serial.print("Error in setInt, ");
    Serial.println(firebaseData1.errorReason());
  }

}
