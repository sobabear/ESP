#include "HX711.h"
 
//찾은 캘리브레이션값을 넣어 줍니다.
#define calibration_factor -18550.0
 
//DT(DOUT)로 사용하는 핀
#define LOADCELL_DOUT_PIN 2
//SCK(CLK)로 사용하는 핀
#define LOADCELL_SCK_PIN 3
 
HX711 scale;
 
//최신값을 몇개 저장해 두는지 개수.
int nValueCount = 10;
//최신값을 저장해 두는 배열. 위의 변수 값과 같은 크기를 설정한다.
float fValue[10];
 
 
void setup() 
{
  Serial.begin(9600);
 
  Serial.println("HX711 kg demo");
 
  //HX711 객체를 초기화 한다.
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  //설정된 캘리브레이션 값을 설정 한다.
  scale.set_scale(calibration_factor);
  //영점 잡기. 현재 값을 0으로 둔다.
  scale.tare();
 
  Serial.println("Readings:");
}
 
void loop() 
{
  Serial.print("Reading: ");
 
  //값 임시 저장
  float fValueTemp = 0.0;
  //값 합산
  float fValueSum = 0.0;
 
  
  //scale.get_units() returns a float
  fValueTemp = scale.get_units();
  //읽은 값을 합산값에 넣어 준다.
  fValueSum = fValueTemp;
  
  int i;
  for(i = 0; i < nValueCount; i = i + 1)
  {
    if(i > 0)
    {//0보다 클때만 계산
      //앞번호일수록 오래된 데이터이다.
      
      //기존에 저장된 데이터 합산
      fValueSum = fValueSum + fValue[i];
 
      //값을 앞으로 한칸씩 민다.
      fValue[i - 1] = fValue[i];
    }
  }
 
  //맨 마지막에 최신값을 넣어 준다.
  fValue[nValueCount - 1] = fValueTemp;
 
  //합산값을 평균을 낸다.
  Serial.print((fValueSum / nValueCount), 1);
  
  //You can change this to kg but you'll need to refactor the calibration_factor
  Serial.print(" kg");
  Serial.println();
}
