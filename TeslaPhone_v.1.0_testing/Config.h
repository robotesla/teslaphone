#define pin_LED_ON      A5              //Пин управления подсветкой дисплея
#define pin_POWER_ON    A4              //Пин управления питанием
#define pin_POWER_KEY   12              //Номер порта кнопки Включения/Отключения питания

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
