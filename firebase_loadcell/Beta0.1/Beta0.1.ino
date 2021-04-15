#include "HX711.h" //HX711로드셀 엠프 관련함수 호출
#define calibration_factor 529.0 // 로드셀 스케일 값 선언

#define DELAY_TIME 1
#define LEN 20

#define DOUT  5 //엠프 데이터 아웃 핀 넘버 선언
#define CLK  4  //엠프 클락 핀 넘버 
#define TAREBTN 2

float tare_value = 468;
float weight = 0;

HX711 scale(DOUT, CLK); //엠프 핀 선언 

void setup() {
  Serial.begin(9600);  // 시리얼 통신 개방 115200으로도 가
  Serial.println("HX711 scale TEST");  
  scale.set_scale(calibration_factor);  //스케일 지정 
  dummy_filter();
  
  scale.tare();  //스케일 설정
  Serial.println("Readings:");
  pinMode(TAREBTN, INPUT);
  Serial.println("Button Start");

}

void loop() {


//  temp_print();
  Serial.print("Reading: ");
  Serial.print(scale.get_units(5), 0);

  Serial.println(" g"); //단위
//  Serial.print("   scale is");Serial.print(scale.get_units(),1);Serial.print("  tare value is :");Serial.print(tare_value);Serial.println();

  delay(500);
  if (digitalRead(TAREBTN) == HIGH) {
    Serial.println("TARE"); 
    taring();


  }
}







void dummy_filter() {
  delay(200);
  float dummy_tare; //정육각 코드에서는 tareindex
  for (int ti = 0; ti < 200; ti++) {
    dummy_tare = scale.get_units();
  }
  delay(200);
}



// TODO: - 영점 조절하는 내용 들어가야함
void taring() {
  float temp;
  
  scale.set_scale();
  scale.tare();
  delay(1000);
  Serial.println("올려줘");
  delay(1000);
  temp = scale.get_units(10);
  scale.set_scale(temp);

  Serial.println();
}

// TODO: - 파이어 베이스에 넣는 내용 들어가야




void temp_print() {
  Serial.print("Reading: ");
  Serial.print(scale.get_units(), 0);

  Serial.println(" g"); //단위
//  Serial.print("   scale is");Serial.print(scale.get_units(),1);Serial.print("  tare value is :");Serial.print(tare_value);Serial.println();

  delay(500);
  
}

void ft_print() {
//    Serial.print("Reading: ");
//  Serial.print(scale.get_units() - tare_value, 1);  //무제 출력 
  weight = scale.get_units();
//  Serial.print("get_units() is ");Serial.println(weight); 
//  Serial.print("read() is "); Serial.print(scale.read());Serial.println();
  Serial.print("get value is "); Serial.print(scale.get_value());Serial.println();
//  Serial.print("get scale is "); Serial.print(scale.get_scale()); Serial.println();
  Serial.print("get offset is "); Serial.print(scale.get_offset()); Serial.println();
  Serial.print("read average is "); Serial.print(scale.read_average(1));Serial.println();
  Serial.print("maybe getbalue is :"); Serial.print(scale.read_average(1) - scale.get_offset());Serial.println();
//  Serial.print(" g"); //단위
//  Serial.print("   scale is");Serial.print(scale.get_units(),1);Serial.print("  tare value is :");Serial.print(tare_value);Serial.println();

 Serial.println();
  delay(1000);
  scale.set_offset(1000);
  
}
