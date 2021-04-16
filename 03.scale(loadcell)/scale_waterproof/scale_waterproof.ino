#include "HX711.h" //HX711로드셀 엠프 관련함수 호출
#define calibration_factor 529.0 // 로드셀 스케일 값 선언
//#define calibration_factor 229.0 
#define DOUT  5 //엠프 데이터 아웃 핀 넘버 선언
#define CLK  4  //엠프 클락 핀 넘버 
#define TAREBTN 2

HX711 scale(DOUT, CLK); //엠프 핀 선언 

void setup() {
  Serial.begin(9600);  // 시리얼 통신 개방
  Serial.println("HX711 scale TEST");  
  scale.set_scale(calibration_factor);  //스케일 지정 
  scale.tare();  //스케일 설정
  Serial.println("Readings:");

}
void loop() {
  Serial.print("Reading: ");
  Serial.print(scale.get_units() - 780, 1);  //무제 출력 
//  Serial.print(scale.read_average(1));
  Serial.print(" g"); //단위
  Serial.println(); 
  delay(500);

  if (digitalRead(TAREBTN) == HIGH) {
    scale.tare();
    Serial.println("TARE 눌렸쩡 ");
  } else {
    Serial.println("안눌렸떵 ");
  } 
}
