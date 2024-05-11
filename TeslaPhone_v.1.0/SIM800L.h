#include <SoftwareSerial.h>

SoftwareSerial SIM800_Serial(3, 2);          // Выводы SIM800L Tx & Rx подключены к выводам Arduino 3 и 2

String Sim800L_DataRead(uint32_t timeout)
{

    uint64_t timeOld = millis();

    while (!SIM800_Serial.available() && !(millis() > timeOld + timeout))
    {
        delay(13);
    }

    String str;

    while(SIM800_Serial.available())
    {
        if (SIM800_Serial.available()>0)
        {
            str += (char) SIM800_Serial.read();
        }
        delay(1);
    }
    return str;
}

void SIM800_ReadAll(void)
{
  if (SIM800_Serial.available()>0)
  {
    while(SIM800_Serial.available())
    {
      SIM800_Serial.read();
    }
  }
}

//void SIM800_DataSend(char *data)
void SIM800_DataSend(String data)
{
  //SIM800_ReadAll();
  SIM800_Serial.println(data);
}

void SIM800_Init(void)
{
  //GSM SerialPort Init
  SIM800_Serial.begin(9600);
}

void SIM800_AT(void)
{
  SIM800_DataSend("AT");
  Sim800L_DataRead(500);
}

bool SIM800_isRing(void)
{
  String _buf = "";
  _buf = Sim800L_DataRead(100);
  //Serial.print(_buf);
  if((_buf.indexOf("RING")) != -1)
  {
    //Serial.print(_buf);
    if((_buf.indexOf("CLIP")) != -1){
      _buf=_buf.substring(_buf.indexOf("\"")+1, _buf.indexOf(",")-1);
      //Serial.println(_buf);
      SIM800.PhoneIncoming = _buf;
      //Serial.println(SIM800.PhoneNumber);
      if(SIM800.Ring == false){
        SIM800.Ring = true;
        Display_IncomingCall();
      }
    }
    return true;
  }else{
    return false;
  }
}

//Проверка регистрации в сети
void SIM800_Reg(void)
{
  String _buf = "";
  SIM800_DataSend("AT+CREG?");
  _buf = Sim800L_DataRead(500);
  #ifdef FLAG_DEBUG
  Serial.print(_buf);
  #endif
  if((_buf.indexOf("OK")) != -1 && (_buf.indexOf("CREG"))!=-1)
  {
    _buf=_buf.substring(_buf.indexOf(",")+1, _buf.indexOf(",")+2);
    if((_buf.indexOf("1")) != -1){
      SIM800.Connect = true;
      #ifdef FLAG_DEBUG
      Serial.println("Reg=OK");
      #endif
    }else{
      #ifdef FLAG_DEBUG
      Serial.println("ERR");
      #endif
    }
  }
}

void SIM800_GSM_Name(void)
{
  if(SIM800.GSM_NameOK == true){return;}
  String _buf = "";
  SIM800_DataSend("AT+COPS?");
  _buf = Sim800L_DataRead(1000);
  #ifdef FLAG_DEBUG
  Serial.print(_buf);
  #endif
  if((_buf.indexOf("OK")) != -1 && (_buf.indexOf("+COPS")) != -1 && _buf.indexOf("\"") != -1)
  {
    _buf=_buf.substring(_buf.indexOf("\"")+1, _buf.lastIndexOf("\""));
    #ifdef FLAG_DEBUG
    Serial.println(_buf);
    #endif
    SIM800.GSM = _buf;
    SIM800.GSM_NameOK = true;
  }
  delay(100);
}

void SIM800_CallEnd(void)
{
  String _buf = "";
  if(SIM800.Ring == true || SIM800.Dial == true || SIM800.Call == true)
  {
    SIM800_DataSend("ATH");
    _buf = Sim800L_DataRead(500);
    #ifdef FLAG_DEBUG
    Serial.print(_buf);
    #endif
    /*if((_buf.indexOf("OK")) != -1)
    {

    }*/
    DateTime_ReView();
    SIM800.Ring = false;
    SIM800.Dial = false;
    SIM800.Call = false;
  }
}

void SIM800_CallAnswer(void)
{
  String _buf = "";
  SIM800_DataSend("ATA");
  _buf = Sim800L_DataRead(500);
  #ifdef FLAG_DEBUG
  Serial.print(_buf);
  #endif
  /*if((_buf.indexOf("OK")) != -1)
  {

  }*/
  Display_PhoneSpeaking();
  SIM800.Dial = false;
  SIM800.Call = true;
}

void SIM800_PhoneNumEnter(void)
{
  Display_OutCall();
  for(uint8_t i=0; i<12; i++){
    SIM800.PhoneNumber[i] = 0x00;
  }
  SIM800.NumCount = 0;
  SIM800.Dial = true;
}

//Позвонить по набранному номеру
void SIM800_PhoneDial(void)
{
  if(SIM800.Call == true){return;}
  //Выход, если номер короткий
  if(SIM800.NumCount<4){return;}
  String _buf = "";
  String arr = "ATD+7" + String(SIM800.PhoneNumber) + ";";
  SIM800.PhoneIncoming = "+7" + String(SIM800.PhoneNumber);
  
  #ifdef FLAG_DEBUG
  Serial.print(arr);
  #endif
  SIM800_DataSend(arr);
  _buf = Sim800L_DataRead(1000);
  #ifdef FLAG_DEBUG
  Serial.print(_buf);
  #endif
  if((_buf.indexOf("OK")) != -1)
  {
    Display_PhoneSpeaking();
    SIM800.Call = true;
  }else{
    DateTime_ReView();
    SIM800.Dial = false;
    SIM800.Call = false;
  }
}

void SIM800_Call(void)
{
  if(SIM800.Connect == false){return;}
  if(SIM800.Ring == true){
    //Ответить на входящий вызов
    SIM800_CallAnswer();
  }else{
    if(SIM800.Dial == false){
      //Набор телефонного номера
      SIM800_PhoneNumEnter();
    }else{
      //Позвонить по набранному номеру
      SIM800_PhoneDial();
    }
  }
}

void SIM800_TimeRead(void)
{
  String _buf = "";
  SIM800_DataSend("AT+CCLK?");
  _buf = Sim800L_DataRead(500);
  #ifdef FLAG_DEBUG
  Serial.print(_buf);
  #endif
  if((_buf.indexOf("OK")) != -1)
  {
    uint8_t day, month, year, hour, minute, second;
    _buf=_buf.substring(_buf.indexOf("\"")+1, _buf.lastIndexOf("\"")-1);
    year=_buf.substring(0,2).toInt();
    month= _buf.substring(3,5).toInt();
    day=_buf.substring(6,8).toInt();
    hour=_buf.substring(9,11).toInt();
    minute=_buf.substring(12,14).toInt();
    second=_buf.substring(15,17).toInt();

    if(day == 0 || month == 0){return;}

    if(DateTime.Hour != hour || DateTime.Min != minute){
      DateTime.TimeReFresh=true;
    }
    if(DateTime.Hour != hour || DateTime.Min != minute){
      DateTime.DateReFresh=true;
    }

    #ifdef FLAG_DEBUG
    char ArrDisp[24];
    sprintf(ArrDisp, "%d.%d.%d %d:%d",year,month,day,hour,minute);
    Serial.print(ArrDisp);
    #endif

    DateTime.Hour = hour;
    DateTime.Min = minute;

    DateTime.Date = day;
    DateTime.Month = month;
    DateTime.Year = year;

    SIM800.DateTimeOK = true;
  }
}

void SIM800_Battery(void)
{
  String _buf = "";
  SIM800_DataSend("AT+CBC");
  _buf = Sim800L_DataRead(500);
  #ifdef FLAG_DEBUG
  Serial.print(_buf);
  #endif
  
  if((_buf.indexOf("OK")) != -1)
  {
    _buf=_buf.substring(_buf.indexOf(",")+1, _buf.lastIndexOf(","));
    #ifdef FLAG_DEBUG
    Serial.println(_buf);
    #endif
    if(SIM800.Battery != _buf.toInt()){
      SIM800.Battery = _buf.toInt();
      DateTime.BatteryReFresh = true;
    }
  }
}

//Аккумулятор разряжен - отключение питания
void SIM800_BatteryIsLow_PowerOff(void)
{
  Display_Clear();
  
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(2);
  tft.setCursor(16, 42);
  tft_print_rus("Аккумулятор");
  tft.setCursor(28, 68);
  tft_print_rus("разряжен!");

  delay(1600);
  Display_Clear();
  digitalWrite(pin_POWER_ON, LOW);
  while(1);
}

void SIM800_BatteryIsLow(void)
{
  static unsigned char cnt = 0;
  if(SIM800.Battery < BatteryLow){
    if(cnt>10){
      cnt=0;
      SIM800_BatteryIsLow_PowerOff();
    }else{
      cnt++;
    }
  }else{
    cnt=0;
  }
}

void SIM800_ReFresh(void)
{
  static uint8_t cnt = 0;
  static uint8_t state = 0;
  static unsigned long time_cnt = millis();
  if(millis() - time_cnt >= 1000) {time_cnt = millis();}else{return;}

  if(SIM800.Connect == false){
    SIM800_Reg();
    if(SIM800.Connect == true){
      SIM800_GSM_Name();
      GSM_NetworkView();

      SIM800_TimeRead();
      Time_View();
      Date_View();

      //SIM800_Battery();
      //Battery_View();
    }
    SIM800_Battery();
    Battery_View();
  }else{
    //Выход, если идет разговор
    if(SIM800.Call == true){return;}
    //Выход, если идет набор номера
    if(SIM800.Dial == true){return;}
    //Исходящий вызов
    if(SIM800_isRing() == true){
      BrightCountClear();
      cnt = 0;
      return;
    }
    if(SIM800.Ring == true){
      cnt++;
      if(cnt > 10){
        SIM800.Ring = false;
        DateTime_ReView();
        state = 0;
      }
      return;
    }

    switch(state)
    {
      case 0:
        SIM800_GSM_Name();
        break;
      case 1:
        SIM800_TimeRead();
        break;
      case 2:
        if(DateTime.TimeReFresh==true){
          DateTime.TimeReFresh=false;
          Time_View();
        }
        break;
      case 3:
        if(DateTime.DateReFresh==true){
          DateTime.DateReFresh=false;
          Date_View();
        }
        break;
      case 4:
        SIM800_Battery();
        break;
      case 5:
        if(DateTime.BatteryReFresh==true){
          DateTime.BatteryReFresh=false;
          //Отобразить текущий заряд аккумулятора
          Battery_View();
          //Проверка разряженности аккумулятора и отключение питания
          SIM800_BatteryIsLow();
        }
        break;
    }

    if(state>5){state=0;}else{state++;}//*/
  }
}

//Обновить данные
void SIM800_StartData(void)
{
  SIM800_Reg();
  SIM800_GSM_Name();
  SIM800_TimeRead();
  SIM800_Battery();
}

void SIM800_StartTest(void)
{
  //AT
  SIM800_AT();
  //Установка для считывания Даты и времени с сохранением в памяти
  SIM800_DataSend("AT+CLTS=1;&W");
  //Установка Громкости звонка
  SIM800_DataSend("AT+CRSL=95");
  //Включить определитель номера
  SIM800_DataSend("AT+CLIP=1");
  //Считать текущий уровень заряда аккумулятора
  SIM800_Battery();
}
