//v.1.0.8_2024.03.21
//#define FLAG_DEBUG                      //Вывод отладочной информации

#define pin_LED_ON      A5              //Пин управления подсветкой дисплея
#define pin_POWER_ON    A4              //Пин управления питанием
#define pin_POWER_KEY   12              //Номер порта кнопки Включения/Отключения питания

#define logo_time       6000            //Время в миллисекундах, которое отображается логотип при включении телефона
#define bright_time     10              //Время в секундах, после которого подсветка дисплея отключается для экономии энергии аккумулятора
#define BatteryLow      7               //Уровень заряда аккумулятора (%) при котором происходит автоматическое отключение питания

#define Color_Tesla     0xA018          //Цвет фона логотипа Tesla

#define Color_DateTime  Color_Tesla    //Цвет отображения Даты и Времени на главном экране
#define Color_GSM       Color_Tesla    //Цвет отображения GSM сети на главном экране
#define Color_Battery   Color_Tesla    //Цвет отображения заряжа аккумулятора на главном экране
#define Color_CallNumber Color_Tesla    //Цвет номера звонящего абонента
//#define Color_DateTime  ST77XX_WHITE    //Цвет отображения Даты и Времени на главном экране
//#define Color_GSM       ST77XX_WHITE    //Цвет отображения GSM сети на главном экране
//#define Color_Battery   ST77XX_WHITE    //Цвет отображения заряжа аккумулятора на главном экране

typedef struct
{
  unsigned TimeReFresh:1;
  unsigned DateReFresh:1;
  unsigned BatteryReFresh:1;

  uint8_t Date;
  uint8_t Month;
  uint8_t Year;

  uint8_t Hour;
  uint8_t Min;
} datetime_t;

typedef struct
{
  unsigned Connect:1;       //Флаг подключения к GSM-сети
  unsigned DateTimeOK:1;    //Флаг корректных принятых данных Даты и Времени
  unsigned GSM_NameOK:1;    //Флаг полученного имени GSM-сети
  unsigned Ring:1;          //Флаг входящего вызова
  unsigned Dial:1;          //Флаг набираемого номера
  unsigned Call:1;          //Флаг вызова набранного номера
  unsigned BrightON:1;      //Флаг включенной подсветки дисплея
  uint8_t NumCount;         //Номер набираемой цифры в телефонном номере
  uint8_t BrightCount;      //Счетчик времени включенной подсветки дисплея
  uint16_t Battery;         //Напряжение на аккумуляторе
  String GSM;               //Имя сети, к которой подключились
  String PhoneIncoming;     //Телефонный номер звонящего абонента
  char PhoneNumber[12];     //Набираемый Телефонный номер для дозвона
} sim800_t;

sim800_t SIM800;

