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
}

//Проверка регистрации в сети
bool SIM800_RegGSM_Wait(void)
{
  String _buf = "";
  SIM800_DataSend("AT+CREG?");
  _buf = Sim800L_DataRead(500);
  if((_buf.indexOf("OK")) != -1 && (_buf.indexOf("CREG"))!=-1)
  {
    _buf=_buf.substring(_buf.indexOf(",")+1, _buf.indexOf(",")+2);
    if((_buf.indexOf("1")) != -1){
      return true;
    }else{
      return false;
    }
  }
}

void SIM800_GSM_Name(void)
{
  if(SIM800.GSM_NameOK == true){return;}
  String _buf = "";
  SIM800_DataSend("AT+COPS?");
  _buf = Sim800L_DataRead(1000);
  //Serial.print(_buf);
  if((_buf.indexOf("OK")) != -1 && (_buf.indexOf("+COPS")) != -1 && _buf.indexOf("\"") != -1)
  {
    _buf=_buf.substring(_buf.indexOf("\"")+1, _buf.lastIndexOf("\""));
    Serial.print("GSM-Name = ");
    Serial.println(_buf);
  }else{
    Serial.println("GSM-Name=ERROR");
  }
  delay(100);
}

void SIM800_TimeRead_Test(void)
{
  String _buf = "";
  SIM800_DataSend("AT+CCLK?");
  _buf = Sim800L_DataRead(500);
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

    char ArrDisp[24];
    sprintf(ArrDisp, "20%02d.%02d.%02d %02d:%02d",year,month,day,hour,minute);
    Serial.print("Time = ");
    Serial.println(ArrDisp);
  }else{
    Serial.println("Time = ERROR");
  }
}

void SIM800_Battery_Test(void)
{
  String _buf = "";
  SIM800_DataSend("AT+CBC");
  _buf = Sim800L_DataRead(500);
  if((_buf.indexOf("OK")) != -1)
  {
    _buf=_buf.substring(_buf.indexOf(",")+1, _buf.lastIndexOf(","));
    if(SIM800.Battery != _buf.toInt()){
      SIM800.Battery = _buf.toInt();
      Serial.print("Battery = ");
      Serial.print(SIM800.Battery);
      Serial.println("\%");
      return;
    }
  }
  Serial.println("Battery = ERROR");
}

void SIM800_Testing(void)
{ 
  String _buf = "";
  Sim800L_DataRead(100);
  //AT
  SIM800_DataSend("AT");
  _buf = Sim800L_DataRead(500);
  if((_buf.indexOf("OK")) != -1)
  {
    Serial.println("Command(AT) = OK");
  }else{
    Serial.println("Command(AT) = ERROR");
  }

  //Ожидание подключения к GSM
  SIM800.Connect=false;
  Serial.print("Wait connect GSM");
  for(uint8_t i = 0; i<30; i++){
    if(SIM800_RegGSM_Wait() == true){
      SIM800.Connect = true;
      Serial.println("\nConnect GSM = OK");
      break;
    }
    Serial.print(".");
    delay(1000);
  }
  if(SIM800.Connect == false){
    Serial.println("Connect GSM = ERROR");
    return;
  }

  //Имя GSM-сети
  SIM800_GSM_Name();

  //Установка для считывания Даты и времени с сохранением в памяти
  SIM800_DataSend("AT+CLTS=1");
  Sim800L_DataRead(500);
  //Считать дату и время
  SIM800_TimeRead_Test();

  //Уровень заряда батареи
  SIM800_Battery_Test();
  
}
