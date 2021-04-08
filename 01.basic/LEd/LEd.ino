#define LED 2 // ESP 보드에 내장된 LED 핀은 2
 int i = 0;

void setup() {
  // put your setup code here, to run once:


  Serial.begin(115200);
  pinMode(LED,OUTPUT);
}

void loop() {
  delay(500);
  digitalWrite(LED,HIGH);
  delay(1500);
  digitalWrite(LED,LOW);
  Serial.print(i);
  Serial.println("working");
  i++;

  // put your main code here, to run repeatedly:

}
