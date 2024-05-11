#include <SoftwareSerial.h>
#include "Config.h"
#include "SIM800L.h"

void POWER_ON(void)
{
  pinMode(pin_LED_ON, OUTPUT);
  
}

void setup(void)
{
  pinMode(13, OUTPUT);
  //Подать питание
  pinMode(pin_POWER_ON, OUTPUT);
  digitalWrite(pin_POWER_ON, HIGH);

  //Serial Init
  Serial.begin(9600);
  Serial.println("===Tesla Phone v.1.0 Testing===");
  delay(1000);
  
  //SIM800 Init
  SIM800_Init();
  delay(100);
  //Отправить команду AT GSM-модулю
  SIM800_AT();
  delay(100);
  SIM800_AT();
  delay(1000);

  //GSM-test
  SIM800_Testing();
}

void loop(void)
{
  delay(1);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);
}