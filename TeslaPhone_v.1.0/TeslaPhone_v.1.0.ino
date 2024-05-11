#include <SoftwareSerial.h>
#include "Config.h"
#include "Display.h"
#include "SIM800L.h"
#include "Keyboard.h"

void setup(void)
{
  //Подать питание
  POWER_ON();

  //TFT Init
  Display_Init();
  //Отобразить логотип
  Logo_View();

  //Serial Init
  Serial.begin(9600);
  Serial.println("===Tesla Phone v.1.0===");
  //SIM800 Init
  SIM800_Init();
  delay(100);
  //Отправить команду AT GSM-модулю
  SIM800_AT();
  
  //Задежка отображения логотипа
  delay(logo_time);
  //Считать параметры с GSM-модуля
  SIM800_StartTest();
  SIM800_StartData();
  delay(1000);
  //Очистить дисплей
  Display_Clear();
  delay(100);
  //SIM800_StartData();
  //Отобразить главный экран
  Display_GlobalViev();
}

void loop(void)
{
  //Опрос нажатия кнопок
  Keys();
  //Обновление данных GSM-модуля
  SIM800_ReFresh();
  //Отключение подсветки дисплея для экономии энергии аккумулятора
  Display_BrightOff();
  
}