
#define TAREBTN 2


void setup() {
  Serial.begin(115200); // 시리얼 포트 오픈
  Serial.println("being");
  pinMode(TAREBTN, INPUT);
  // put your setup code here, to run once:

}

void loop() {

  // 풀업 저항이라 
  if (digitalRead(TAREBTN) == HIGH) {
    Serial.println("TARE 눌렸쩡 ");
  } else {
    Serial.println("안눌렸떵 ");
  }
  delay(100);

}
