#include <Keypad.h>

char MatrixKeyPad[4][4] = {
      {'<','>','Z','X'},
      {'3','6','9','#'},
      {'2','5','8','0'},
      {'1','4','7','*'}};

byte rowPins[4] = {10, 8, 7, 4}; // Задаем пины строк (R1, R2, R3, R4)
byte colPins[4] = {A0, 5, 6, 9}; // Задаем пины столбцов (L4, L3, L2, L1)

// создаем переменную типа класс библиотеки Keypad
Keypad customKeypad = Keypad(makeKeymap(MatrixKeyPad), colPins, rowPins, 4, 4);

/*void Battery_TestView(void)
{
  static uint8_t tmp = 0;
  tmp+=5;
  if(tmp>100){tmp=0;}
  Battery_Set(tmp);
}*/

void POWER_ON(void)
{
  pinMode(pin_LED_ON, OUTPUT);
  pinMode(pin_POWER_ON, OUTPUT);
  pinMode(pin_POWER_KEY, INPUT);
  digitalWrite(pin_POWER_ON, HIGH);
  Bright_ON();
}
void POWER_OFF(void)
{
  Display_Clear();
  digitalWrite(pin_POWER_ON, LOW);
  
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(2);
  tft.setCursor(20, 58);
  tft_print_rus("Выключение");
  delay(1200);
  Display_Clear();
  while(1);
}

void KeyPower(void)
{
  static bool press=false;
  static uint8_t cnt = 0;
  static unsigned long time_cnt = millis();
  if(millis() - time_cnt >= 100) {time_cnt = millis();}else{return;}
  if(digitalRead(pin_POWER_KEY)==LOW){
    BrightCountClear();
    if(press == false){
      press = true;
    }else{
      cnt++;
      if(cnt>=25){
        //Отключить питание
        POWER_OFF();
      }
    }
  }else{
    if(press == true){
      if(cnt<20){
        //Ответить на входящий вызов или начать вызов абонента
        SIM800_Call();
      }
      press = false;
      cnt = 0;
    }
  }
}

void Keys(void)
{
  char customKey = customKeypad.getKey(); 
  //Сброс счетчика времени отключения подсветки дисплея при нажатии на любую кнопку
  if(customKey != '\0'){
    BrightCountClear();
  }
    //Serial.print(customKey + "\r\n");
  //Обработка нажатия кнопки включения
  KeyPower();
  //Обработка нажатий цифровых кнопок
  if(customKey>='0' && customKey<='9' && SIM800.Dial == true){
    if(SIM800.NumCount < 10){
      //Отобразить на дисплее введенную цифру
      tft.print(customKey);
      //Добавление введенного числа к телефонному номеру
      //SIM800.PhoneNumber += customKey;
      SIM800.PhoneNumber[SIM800.NumCount] = customKey;
      //Увеличение длины набираемого телефонного номера
      SIM800.NumCount++;
    }
  }
  else if(customKey == 'Z')   //Кнопка "Отмена"
  {
    //Key=>CallEnd
    SIM800_CallEnd();
  }
  else  //Остальные кнопки
  {
    switch(customKey){
      case '<':
        //Serial.println("<");
        //Стирание последней(ошибочно) введенной цифры
        if(SIM800.Dial == true){
          if(SIM800.NumCount>0 && SIM800.NumCount<=10)
          {
            //Уменьшить длину набираемого номера
            SIM800.NumCount--;
            //Стереть последнюю цифру с экрана
            tft.fillRect(6+24 + SIM800.NumCount*12, 84, 12, 14, ST77XX_BLACK);
            //Сместить позицию курсора назад на один символ
            tft.setCursor(6 + 24 + SIM800.NumCount*12, 84);
            //Удалить последнюю цифру из телефонного номера
            SIM800.PhoneNumber[SIM800.NumCount] = 0x00;
          }
        }
        break;
      case '>':
        //Serial.println(">");
        break;
      case '*':
        //Serial.println("*");
        break;
      case '#':
        //Serial.println("#");
        break;
    }//*/
  }
}
