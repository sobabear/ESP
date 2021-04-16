#include <HX711.h>
/*
  성남 공장 라벨 저울 - 예비용 model : wz-2d
*/
//
//#include "HX711.h"
#define calibration_factor 592000.8 //224.75 // 테스트 해가면서 조절한 초기 조정값
#define DOUT 5
#define CLK 4
#define TAREBTN 2
#define LEN 5
#define DELAY_TIME 1
#define MAX_WEIGHT 6000000

bool scaleflag = false;
int i;
float scalelist[LEN];
float tarevalue;

bool weightflag;
int output_weight;

boolean tare_flag = false;
float zero_data = 0;
HX711 scale(DOUT, CLK);

void custom_tare() {
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
        if (abs(custom_scalelist[j] - ave) < 1500.0) {
          changeflag = true;
        } else {
          changeflag = false;
        }
      }
      if (changeflag) {
        //   zeroflag = false;
        tarevalue = ave;
        break;
      }
    }
  }
}


void setup() {
  Serial.begin(115200); // 시리얼 포트 오픈
  pinMode(TAREBTN, INPUT);
  boolean a;
  while (!(a = scale.is_ready())) {
//       Serial.println(a);
  }
  scale.set_scale();
  delay(100);
  // HX-711 영점조절
  i = 0;
  scaleflag = false;

  // 아두이노가 처음 실행되면 HX-711 또는 로드셀에서 일정시간동안은 안정화되지 않은 값이 들어오므로
  // 안정화될동안 쓰레기값 버리기

  float tareindex;

  for (int ti = 0; ti < 20; ti++) {
    tareindex = scale.get_units(1);
  }

  tareindex = 0.0; // 쓰레기값 버리기 완료
  int tarecount = 0;
  custom_tare();
  weightflag = false; // 무게 인식 Flag
}

float custom_read() {
  float data = 0.0;
  data = scale.read_average(1) - tarevalue;
  data = data / calibration_factor;
  return data;
}


void loop() {
  float scaleResult = custom_read();
  char text = 't';
  char serialcmd = Serial.read();

  if (digitalRead(TAREBTN) == HIGH || text == serialcmd) { //통신으로 tare명령 제어.
    Serial.println("TARE");
    tare_flag = false;
    custom_tare();
  }

  if (scaleResult < -500) {
    tare_flag = true;
  }
  if (!tare_flag) {
    if (scaleResult < MAX_WEIGHT && scaleResult > -10000) {
      scalelist[i] = scaleResult;
      i++;
      if (i > LEN - 1) {
        i = 0;
        scaleflag = true;
      } // 여기까지가 20개의 값을 배열에 계속새서 저장하면서 한칸씩 이동한 평균을 계산하기 위해 배열에 저장하는 곳

      if (scaleflag) { // 처음에 20개의 값이 저장되기 전까지는 scaleflag를 두어서 계산 방지
        float sum = 0;
        int max_a = 0, min_a = 0;
        for (int i = 0; i < LEN; i++) {
          sum = sum + scalelist[i];
        }

        float ave = (float) (sum) / LEN; //20개의 값의 평균값 산출
        bool changing = false;
         Serial.print(ave); Serial.println();
   
        for (int i = 0; i < LEN; i++) {
          if (abs(scalelist[i] - ave) > 1.5) {
            changing = true;
          }
        }
//
//        if (ave < output_weight / 2) {
//          Serial.print("z"); Serial.println();
//          weightflag = false;
//        }
        if (!changing) { // 무게변동 없음
          if (scaleResult < 30.0) { // 측정된 평균값이 0.05 이하면 아무것도 안올려둔것과 동일함
            Serial.print("z"); Serial.println();
            zero_data = ave;
            weightflag = false; // 무게측정 flag 초기화
          } else {
            if (!weightflag) { // 무게측정 flag가 초기화 되어있다가 무게가 올려진 상태
              output_weight = ave - zero_data;
              weightflag = true;
              // 변경된 무게를 무시하기 위해 따로 저장해두고 무게측정 flag 초기화 전까는 이 무게를 계속 출력
              Serial.print("a"); Serial.print(output_weight); Serial.print("g"); Serial.println();
            } else { // 이미 한번 무게가 올라간 상태에서 계속해서 무게가 측정된 상태
              Serial.print("a"); Serial.print(output_weight); Serial.print("g"); Serial.println();
            }
          }
        } else { // 무게변동중
          //        Serial.print("c"); Serial.print(ave * 990); Serial.print("c"); Serial.println();
        }
      }
    }
  } else {
    tare_flag = false;
  }
}
