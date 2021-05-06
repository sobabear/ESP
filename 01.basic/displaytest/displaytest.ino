#include <HT1621.h>

HT1621 lcd;

int CS = 2;
int WR = 3;
int DATA = 4;
int LED = 5;

int i = 0;
void setup() {
  // put your setup code here, to run once:

  lcd.begin(CS, WR, DATA, LED);
  lcd.noBacklight();
  lcd.clear();

}

void loop() {
  // put your main code here, to run repeatedly:

  lcd.setBatteryLevel(1);
  delay(1000);

// lcd.print(i, 1);

  lcd.setBatteryLevel(3);
 delay(1000);
 i++;
}
