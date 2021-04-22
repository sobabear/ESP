/**
 *
 * 
 * https://github.com/stareta1202/ESP
 *
 * MIT License
 * (c) 2021 Yongjun Lee
 *
 * 통신 관련 -> Task async
 * 저울 딴 -> loop(Task main)
 * 
**/



#include "HX711.h"
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <time.h>

#define WIFI_SSID "JYGLAB_2.4G"
#define WIFI_PASSWORD "18000658"
#define MAX_WEIGHT 6000000


//개인 테스트용
//#define FIREBASE_HOST "https://ygfirebasetest-default-rtdb.firebaseio.com/"
//#define FIREBASE_AUTH "QKxCxRK3tbuKgXFwzbIddadtjx61L3BaqCgKuDok"
//기업 테스트용
 #define FIREBASE_HOST "https://jyg-custom.firebaseio.com"
 #define FIREBASE_AUTH "8thrZQjcL0Cib0Yj3b3ysLVsbBpRrCa4tgSSKSXg"



//Firebase and Wifi
FirebaseData firebaseData;
FirebaseAuth mAuth;
FirebaseJson json;
FirebaseJsonArray jsonArray;

FirebaseConfig mConfig;
String mRef = "/automate_factory/test/pi1/scale";
String mDummyRef = "/automate_factory/test/pi1";

// Core
TaskHandle_t async;
//TaskHandle_t main - > loop()


// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 5;
const int LOADCELL_SCK_PIN = 4;
const int TAREBTN = 2;
const int LEN = 5;
const int DELAY_TIME = 1;
float cal = 526.7;
float offset, zero_data = 0;
boolean tare_flag = false, scaleflag = false, weightflag;
float scalelist[LEN];
int i, output_weight;
int scale_output_weight = 0;
HX711 scale;

long int raw_time;



void setup() {
  Serial.begin(9600);
  setup_wifi();
  setup_firebase();
  setup_scale();
  jyg_tare();

  loop2();

}

void loop() {
  // float scale_result = jyg_get_units();
  char serialcmd = Serial.read();
  if (digitalRead(TAREBTN) == HIGH || serialcmd == 't') {
    Serial.println("눌렸졍");
    jyg_tare();
  }
  // check_valid(scale_result);
}

void loop2() {
  xTaskCreatePinnedToCore(
    asyncThread,
    "async",
    8192,
    NULL,
    0,
    &async,
    0
  );
}

void asyncThread(void *param) {
  while(1) {
     float scale_result = jyg_get_units();
     check_valid(scale_result);
  }
}


// --------------------------------------------------------------
// --------------------- custom functions -----------------------
// --------------------------------------------------------------  
// ========================set up================================

void setup_wifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connectiong to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Coneected with IP: ");
  Serial.println(WiFi.localIP());
  delay(50);
}

void setup_firebase() {
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true); 
  Firebase.setReadTimeout(firebaseData, 1000 * 60);   // Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setwriteSizeLimit(firebaseData, "tiny");   //Size and its write timeout e.g. tiny(1s), small (10s), medium (30s) and large (60s).
}

// ======================== write ===============================

void temp_write_firebase(int weight) {
  int count = 0;
  for(;;) {
    json.set("/value", count);
    Firebase.updateNode(firebaseData, mRef, json);
    count++;
    delay(500);
  }
}

void write_firebase(int weight) {
  Firebase.setTimestamp(firebaseData, mDummyRef + "/rawtimestamp");
  String mTime = unixTimeToHumanReadable(firebaseData.intData());

  json.set("/value",weight);
  json.set("/timestamp",mTime);
  Firebase.updateNode(firebaseData, mRef, json);
  delay(8);
}
void write_scale_firebase(int weight) {

  json.set("/value", weight);
  Firebase.updateNode(firebaseData, mRef, json);
//  json.set("/value", 400);
//  Firebase.updateNode(firebaseData, mRef, json);
  delay(8);
}

void write_timestamp_firebase() {
  Firebase.setTimestamp(firebaseData, mRef + "/rawtimestamp");
  String time = unixTimeToHumanReadable(firebaseData.intData());
  json.set("/timestamp", time);
  Firebase.updateNode(firebaseData, mRef, json);
  delay(8);
}


void check_valid(float result) {
  //시리얼 통신 제어는 추후에 구현 
  if (result < -500) {
    tare_flag = true;
  }
  if (!tare_flag) {
    if (result < MAX_WEIGHT && result > -10000) {
      scalelist[i] = result;
      i++;
      if (i > LEN - 1) {
        i = 0;
        scaleflag = true;
      }
      if (scaleflag) {
        float sum = 0;
        int max_a = 0, min_a = 0;
        for (int j = 0; j < LEN; j++) {
          sum = sum + scalelist[j];
        }
        float ave = (float) (sum) / LEN;
        bool changing = false;
        Serial.print(ave); Serial.println();
        for (int i = 0; i < LEN; i++) {
          if (abs(scalelist[i] - ave) > 2) {
            changing = true;
          }
        }
        if (!changing) {
          if (result < 30.0) {// 측정된 평균값이 30  이하면 아무것도 안올려둔것과 동일
            Serial.print("z"); Serial.println();
            zero_data = ave;
            weightflag = false; // 무게 측정 초기화 
          } else {
            if (!weightflag) { // 무게 측정 flag가 초기화 되어있다가 무게가 올려진 상
              output_weight = ave - zero_data;
              weightflag = true;
              // 변경된 무게를 무시하기 위해 따로 저장해두고 무게 측정 flag 초기화 전까지는 이 무게를 계속 출력
              Serial.print("a");Serial.print(output_weight); Serial.print("g");Serial.println();
              write_firebase(output_weight);
            } else { // 이미 한번 무게가 올라간 상태에서 계속 해서 무게가 측정된 상
              Serial.print("a"); Serial.print(output_weight); Serial.print("g");Serial.println();
            }
          }
          
        } else { // 무게 변동중
          Serial.print("c");Serial.print(ave * 990); Serial.print("c"); Serial.println();
        }
      }
    }
    
  } else {
    tare_flag = false;
  }
  delay(50);
  
}

void jyg_tare() {
  int i = 0;
  float tareindex = 0.0, ave = 0.0;
  boolean flag = false;
  boolean changeflag = false;
  float custom_scalelist[LEN];
  while (true) {
    float tareio =  scale.read_average(1);
    delay(DELAY_TIME);
    custom_scalelist[i] = tareio;
    i++;
    if (i > LEN - 1) {
      i = 0;
      flag = true;
    } // 여기까지가 20개의 값을 배열에 계속새서 저장하면서 한칸씩 이동한 평균을 계산하기 위해 배열에 저장하는 곳

    if (flag) { // 처음에 20개의 값이 저장되기 전까지는 scaleflag를 두어서 계산 방지
      float sum = 0.0;
      for (int si = 0; si < LEN; si++) {
        sum += custom_scalelist[si];
      }
      ave = (float) (sum) / LEN; //20개의 값의 평균값
      for (int j = 0; j < LEN; j++) {
        if (abs(custom_scalelist[j] - ave) < 1300.0) {
          changeflag = true;
        } else {
          changeflag = false;
        }
      }
      if (changeflag) {
        //   zeroflag = false;
        offset = ave;
        break;
      }
    }
  }
  delay(1);
} 

float jyg_get_units() {
  float data = 0;
  data = scale.read_average(1) - offset;
  data = data / cal;
  return data;
}



// ----------------------- 예제에 있던 애들  ㅇ ----------------------
void ft_print() {
  Serial.print("one reading:\t");
  Serial.print(scale.get_units(), 1);
  Serial.print("\t| average:\t");
  Serial.print(scale.get_units(10), 1);
  Serial.print("\t| jygvalue is :\t");
  Serial.println(jyg_get_units(), 0);

  delay(1000);  
}

void setup_scale() {
  Serial.println("HX711 Demo");Serial.println("Initializing the scale");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");Serial.println(scale.read());      // print a raw reading from the ADC
  Serial.print("read average: \t\t");Serial.println(scale.read_average(20));   // print the average of 20 readings from the ADC
  Serial.print("get value: \t\t");Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight (not set yet)
  Serial.print("get units: \t\t");Serial.println(scale.get_units(5), 1);  // print the average of 5 readings from the ADC minus tare weight (not set) divided// by the SCALE parameter (not set yet)
  scale.set_scale(cal); // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();               // reset the scale to 0

  Serial.println("After setting up the scale:");
  Serial.print("read: \t\t");Serial.println(scale.read());                 // print a raw reading from the ADC
  Serial.print("read average: \t\t");Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC
  Serial.print("get value: \t\t");Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight, set with tare()
  Serial.print("get units: \t\t");Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided
            // by the SCALE parameter set with set_scale
  float tareindex;
  for (int ti = 0; ti < 20; ti++) {
    tareindex = scale.get_units(1);
  }
  delay(100);
  Serial.println("Readings:"); 
}


String unixTimeToHumanReadable(long int timestamp)
{
    long int seconds = timestamp + 32400;
 
    // Save the time in Human
    // readable format
    String ans = "";
 
    // Number of days in month
    // in normal year
    int daysOfMonth[] = { 31, 28, 31, 30, 31, 30,
                          31, 31, 30, 31, 30, 31 };
 
    long int currYear, daysTillNow, extraTime,
        extraDays, index, date, month, hours,
        minutes, secondss, flag = 0;
 
    // Calculate total days unix time T
    daysTillNow = seconds / (24 * 60 * 60);
    extraTime = seconds % (24 * 60 * 60);
    currYear = 1970;
 
    // Calculating currrent year
    while (daysTillNow >= 365) {
        if (currYear % 400 == 0
            || (currYear % 4 == 0
                && currYear % 100 != 0)) {
            daysTillNow -= 366;
        }
        else {
            daysTillNow -= 365;
        }
        currYear += 1;
    }
 
    // Updating extradays because it
    // will give days till previous day
    // and we have include current day
    extraDays = daysTillNow + 1;
 
    if (currYear % 400 == 0
        || (currYear % 4 == 0
            && currYear % 100 != 0))
        flag = 1;
 
    // Calculating MONTH and DATE
    month = 0, index = 0;
    if (flag == 1) {
        while (true) {
            if (index == 1) {
                if (extraDays - 29 < 0)
                    break;
                month += 1;
                extraDays -= 29;
            } else {
                if (extraDays - daysOfMonth[index] < 0) {break;}
                month += 1;
                extraDays -= daysOfMonth[index];
            }
            index += 1;
        }
    }
    else {
        while (true) {
            if (extraDays - daysOfMonth[index] < 0) {
                break;
            }
            month += 1;
            extraDays -= daysOfMonth[index];
            index += 1;
        }
    }
 
    // Current Month
    if (extraDays > 0) {
        month += 1;
        date = extraDays;
    }
    else {
        if (month == 2 && flag == 1)
            date = 29;
        else {
            date = daysOfMonth[month - 1];
        }
    }
 
    // Calculating HH:MM:YYYY
    hours = extraTime / 3600;
    minutes = (extraTime % 3600) / 60;
    secondss = (extraTime % 3600) % 60;
 
    ans += String(currYear) + "-" + String(month) + "-" + String(date);
    ans += "T";
    ans += String(hours);
    ans += ":";
    ans += String(minutes);
    ans += ":";
    ans += String(secondss);
 
    // Return the time
    return ans;
}
