#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

#define TFT_CS        A3
#define TFT_DC        A2
#define TFT_RST       A1 // Or set to -1 and connect to Arduino RESET pin

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

datetime_t DateTime;

String utf8rus(String source)
{
  int i,k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  k = source.length(); i = 0;

  while (i < k) {
    n = source[i]; i++;

    if (n >= 0xBF){
      switch (n) {
        case 0xD0: {
          n = source[i]; i++;
          if (n == 0x81) { n = 0xA8; break; }
          if (n >= 0x90 && n <= 0xBF) n = n + 0x2F;
          break;
        }
        case 0xD1: {
          n = source[i]; i++;
          if (n == 0x91) { n = 0xB7; break; }
          if (n >= 0x80 && n <= 0x8F) n = n + 0x6F;
          break;
        }
      }
    }
    m[0] = n; target = target + String(m);
  }
  return target;
}

void tft_print_rus(String source)
{
  //tft.println(utf8rus("абвгдеёжзиклмн"));
  tft.print(utf8rus(source));
}

void Display_Init(void)
{
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
}

void Display_Clear(void)
{
  tft.fillScreen(ST77XX_BLACK);
}

void Bright_ON(void)
{
  if(SIM800.BrightON == true){return;}
  digitalWrite(pin_LED_ON, HIGH);
  SIM800.BrightON = true;
}
void Bright_OFF(void)
{
  if(SIM800.BrightON == false){return;}
  digitalWrite(pin_LED_ON, LOW);
  SIM800.BrightON = false;
}
void BrightCountClear(void){
  SIM800.BrightCount = 0;
  Bright_ON();
}

void Display_BrightOff(void)
{
  static unsigned long time_cnt = millis();
  if(millis() - time_cnt >= 1000) {time_cnt = millis();}else{return;}
  if(SIM800.Ring == true){return;}
  if(SIM800.Call == true){return;}
  if(SIM800.BrightON == false){return;}
  if(SIM800.BrightCount > bright_time){
    Bright_OFF();
    SIM800.BrightCount = 0;
  }else{
    SIM800.BrightCount++;
  }
}

void Logo_View(void)
{
  int16_t x=10, y=3;
  uint16_t color = Color_Tesla;

  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  
  //Logo
  tft.fillTriangle(x+0, y+0, x+32, y+20, x+0, y+13, color);
  tft.fillTriangle(x+0, y+13, x+32, y+20, x+32, y+34, color);

  tft.fillTriangle(x+0, y+27, x+11, y+35, x+0, y+53, color);
  tft.fillTriangle(x+0, y+53, x+11, y+35, x+11, y+60, color);

  tft.fillTriangle(x+21, y+41, x+32, y+48, x+21, y+66, color);
  tft.fillTriangle(x+21, y+66, x+32, y+48, x+32, y+73, color);

  //Tesla
  tft.setCursor(56, 26);
  tft.setTextColor(color);
  tft.setTextSize(3);
  tft.print("TESLA");

  //Phone
  tft.setCursor(56, 58);
  tft.setTextColor(color);
  tft.setTextSize(3);
  tft.print("Phone");

  //Version
  tft.setCursor(20, 100);
  tft.setTextColor(ST77XX_ORANGE);
  tft.setTextSize(2);
  //tft.print("version 1.0");
  tft_print_rus("версия 1.0");
}

void Battery_View(void)
{
  uint16_t x=114, y=2;
  uint16_t color = Color_Battery;
  uint16_t color_current = Color_Battery;

  if(SIM800.Battery == 0){return;}

  if(SIM800.Battery<10){color_current = ST77XX_RED;}
  else if(SIM800.Battery<25){color_current = ST77XX_YELLOW;}

  //Fill
  tft.drawRect(x, y, 20, 12, color_current);
  tft.drawRect(x-3, y+3, 4, 6, color_current);
  //Blocks
  if(SIM800.Battery>10){color = color_current;}else{color = ST77XX_BLACK;}
  tft.fillRect(x+15, y+3, 2, 6, color);
  if(SIM800.Battery>25){color = color_current;}else{color = ST77XX_BLACK;}
  tft.fillRect(x+11, y+3, 2, 6, color);
  if(SIM800.Battery>50){color = color_current;}else{color = ST77XX_BLACK;}
  tft.fillRect(x+7, y+3, 2, 6, color);
  if(SIM800.Battery>75){color = color_current;}else{color = ST77XX_BLACK;}
  tft.fillRect(x+3, y+3, 2, 6, color);

  //%  
  tft.fillRect(x+22, y+2, 24, 8, ST77XX_BLACK);
  tft.setCursor(x+22, y+2);
  tft.setTextColor(color_current);
  tft.setTextSize(1);
  tft.print(SIM800.Battery);
  tft.print("%");
}

void Antenna_View(void)
{
  uint16_t x=2, y=2;

  tft.fillRect(x, y+8, 2, 4, Color_GSM);
  tft.fillRect(x+4, y+6, 2, 6, Color_GSM);
  tft.fillRect(x+8, y+4, 2, 8, Color_GSM);
  tft.fillRect(x+12, y+2, 2, 10, Color_GSM);
}

void GSM_NetworkView(void)
{
  uint16_t x=19, y=6;
  tft.fillRect(x, y, 60, 8, ST77XX_BLACK);
  tft.setCursor(x, y);
  tft.setTextColor(Color_GSM);
  tft.setTextSize(1);

  if(SIM800.Connect == false){
    //tft_print_rus("Поиск сети");
    tft_print_rus("Нет сети");
  }if(SIM800.GSM_NameOK == true){
    tft.print(SIM800.GSM);
  }
}

void Time_View(void)
{
  char ArrDisp[8]={0};
  tft.fillRect(22, 40, 118, 28, ST77XX_BLACK);
  tft.setCursor(22, 40);
  tft.setTextColor(Color_DateTime);
  tft.setTextSize(4);
  
  if(SIM800.DateTimeOK == true){
    sprintf(ArrDisp, "%02d:%02d", DateTime.Hour, DateTime.Min);
    tft.print(ArrDisp);
  }else{
    //Если нет подключения к GSM-сети
    tft.print("--:--");
  }
}

void Date_View(void)
{
  char ArrDisp[16]={0};
  tft.fillRect(22, 80, 118, 16, ST77XX_BLACK);
  tft.setCursor(22, 80);
  tft.setTextColor(Color_DateTime);
  tft.setTextSize(2);
  //tft.print("07.03.2024");

  if(SIM800.DateTimeOK == true){
    sprintf(ArrDisp, "%02d.%02d.20%02d", DateTime.Date, DateTime.Month, DateTime.Year);
    tft.print(ArrDisp);
  }else{
    //Если нет подключения к GSM-сети
    tft.print("--.--.----");
  }
}

void DateTime_ReView(void)
{
  tft.fillRect(2, 40, 154, 70, ST77XX_BLACK);
  Time_View();
  Date_View();
}

void Display_IncomingCall(void)
{
  tft.fillRect(2, 40, 154, 70, ST77XX_BLACK);
  
  tft.setTextColor(Color_CallNumber);
  tft.setTextSize(2);
  tft.setCursor(35, 40);
  tft_print_rus("Входящий");
  tft.setCursor(54, 58);
  tft_print_rus("вызов");
  tft.setCursor(6, 84);
  tft.print(SIM800.PhoneIncoming);
}

void Display_OutCall(void)
{
  tft.fillRect(2, 40, 154, 70, ST77XX_BLACK);
  
  tft.setTextColor(Color_CallNumber);
  tft.setTextSize(2);
  tft.setCursor(30, 40);
  tft_print_rus("Исходящий");
  tft.setCursor(54, 58);
  tft_print_rus("вызов");
  tft.setCursor(6, 84);
  tft.print("+7");
  //<<Набор номера вызываемого абонента кнопками 0..9
}

void Display_PhoneSpeaking(void)
{
  tft.fillRect(2, 40, 154, 70, ST77XX_BLACK);
  
  tft.setTextColor(Color_CallNumber);
  tft.setTextSize(2);
  tft.setCursor(36, 48);
  tft_print_rus("Разговор");
  tft.setCursor(48, 58);

  //Номер телефона абонента (Набранный или Входящий)
  tft.setCursor(6, 84);
  tft.print(SIM800.PhoneIncoming);
}

void Display_GlobalViev(void)
{
  //Time
  Time_View();
  //Date
  Date_View();
  //Battery
  Battery_View();
  //Antenna
  Antenna_View();
  //GSM-Network
  GSM_NetworkView();

  //Find GSM-Network
  //GSM_Find_View();  
}
