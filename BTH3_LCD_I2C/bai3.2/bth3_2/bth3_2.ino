#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

#define PIN_BTN_MODE 2
#define PIN_BTN_UP 3
#define PIN_BTN_DOWN 4
#define PIN_BTN_SET 5
#define PIN_LED 7

LiquidCrystal_I2C lcd(0x27, 16, 2); 
RTC_DS1307 rtc;

enum ModeState {
  MODE_VIEW_TIME,
  MODE_SET_ALARM_H,
  MODE_SET_ALARM_M,
  MODE_SET_TIME_H,
  MODE_SET_TIME_M
};

ModeState currentMode = MODE_VIEW_TIME;

int alarmH = 0;
int alarmM = 0;
bool isAlarmOn = 0;
bool isRing = false;

int tempH, tempM;

unsigned long modePressTime = 0;
bool modeButtonHold = false;

void Buttons()
{
  if (digitalRead(PIN_BTN_MODE) == LOW)
  {
    if (modePressTime == 0) modePressTime = millis();
    if (millis() - modePressTime > 2000 && !modeButtonHold && currentMode == MODE_VIEW_TIME)
    {
      isAlarmOn = !isAlarmOn; 
      modeButtonHold = true; 
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(isAlarmOn ? "Bat bao thuc    " : "Tat bao thuc    ");
      delay(1000);
      lcd.clear();
    }
  }
  else 
  {
    if (modePressTime > 0)
    {
      if (!modeButtonHold && millis() - modePressTime < 1000)
      {
        if (isRing)
        {
          stopAlarm(); 
        }
        else
        {
          changeMode();
        }
      }
      modePressTime = 0;
      modeButtonHold = false;
    }
  }

  if (digitalRead(PIN_BTN_UP) == LOW)
  {
    UpTime();
    delay(200);
  }

  if (digitalRead(PIN_BTN_DOWN) == LOW)
  {
    DownTime();
    delay(200);
  }

  if (digitalRead(PIN_BTN_SET) == LOW)
  {
    SetTimeMode();
    delay(300);
  }
}

void changeMode()
{
  lcd.clear();
  switch (currentMode)
  {
    case MODE_VIEW_TIME:
      currentMode = MODE_SET_ALARM_H;
      tempH = alarmH;
      tempM = alarmM;
      break;
    case MODE_SET_ALARM_H: 
    case MODE_SET_ALARM_M:
      currentMode = MODE_SET_TIME_H;
      {
        DateTime now = rtc.now();
        tempH = now.hour();
        tempM = now.minute();
      }
      break;
    default:
      currentMode = MODE_VIEW_TIME;
      break;
  }
}

void UpTime()
{
  if (currentMode == MODE_SET_ALARM_H || currentMode == MODE_SET_TIME_H)
  {
    tempH++;
    if (tempH > 23) tempH = 0;
  }
  else if (currentMode == MODE_SET_ALARM_M || currentMode == MODE_SET_TIME_M)
  {
    tempM++;
    if (tempM > 59) tempM = 0;
  }
}

void DownTime()
{
  if (currentMode == MODE_SET_ALARM_H || currentMode == MODE_SET_TIME_H)
  {
    tempH--;
    if (tempH < 0) tempH = 23;
  }
  else if (currentMode == MODE_SET_ALARM_M || currentMode == MODE_SET_TIME_M)
  {
    tempM--;
    if (tempM < 0) tempM = 59;
  }
}

void SetTimeMode()
{
  DateTime now = rtc.now();
  switch (currentMode)
  {
    case MODE_SET_ALARM_H:
      currentMode = MODE_SET_ALARM_M; 
      break;
    case MODE_SET_ALARM_M:
      alarmH = tempH; 
      alarmM = tempM;
      isAlarmOn = true; 
      currentMode = MODE_VIEW_TIME;
      lcd.clear();
      lcd.print("Da dat bao thuc!");
      delay(1000);
      lcd.clear();
      break;
    case MODE_SET_TIME_H:
      currentMode = MODE_SET_TIME_M; 
      break;
    case MODE_SET_TIME_M:
      rtc.adjust(DateTime(now.year(), now.month(), now.day(), tempH, tempM, 0));
      currentMode = MODE_VIEW_TIME;
      lcd.clear();
      lcd.print("Da dat lai gio! ");
      delay(1000);
      lcd.clear();
      break;
  }
}

void printDigits(int digits)
{
  if (digits < 10) lcd.print('0');
  lcd.print(digits);
}

void displayRealTime()
{
  DateTime now = rtc.now();
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  printDigits(now.hour()); lcd.print(":");
  printDigits(now.minute()); lcd.print(":");
  printDigits(now.second());

  lcd.setCursor(0, 1);
  lcd.print("Alarm: ");
  if (isAlarmOn) lcd.print("ON ");
  else lcd.print("OFF");
  if (isAlarmOn)
  {
    printDigits(alarmH); lcd.print(":");
    printDigits(alarmM);
  }
}

void displaySetAlarm()
{
  lcd.setCursor(0, 0);
  lcd.print("Dat bao thuc");
  lcd.setCursor(0, 1);
  
  if (currentMode == MODE_SET_ALARM_H) lcd.print(">"); 
  else lcd.print(" ");
  printDigits(tempH);
  lcd.print(":");
  
  if (currentMode == MODE_SET_ALARM_M) lcd.print(">"); 
  else lcd.print(" ");
  printDigits(tempM);
}

void displaySetTime()
{
  lcd.setCursor(0, 0);
  lcd.print("Sua thoi gian");
  lcd.setCursor(0, 1);
  
  if (currentMode == MODE_SET_TIME_H) lcd.print(">");
  else lcd.print(" ");
  printDigits(tempH);
  lcd.print(":");
  
  if (currentMode == MODE_SET_TIME_M) lcd.print(">");
  else lcd.print(" ");
  printDigits(tempM);
}

void checkAlarm()
{
  if (!isAlarmOn) return;
  
  DateTime now = rtc.now();
  if (now.hour() == alarmH && now.minute() == alarmM && now.second() == 0)
  {
    isRing = true;
  }
} 

void ringAlarm()
{
  lcd.setCursor(0, 0);
  lcd.print("Tut Tut Tut!!!  ");
  lcd.setCursor(0, 1);
  lcd.print("Bam MODE de tat ");
  
  digitalWrite(PIN_LED, HIGH);
  delay(100);
  digitalWrite(PIN_LED, LOW);
  delay(100);
}

void stopAlarm()
{
  isRing = false;
  digitalWrite(PIN_LED, HIGH);
  lcd.clear();
}

void setup() {
  pinMode(PIN_BTN_MODE, INPUT_PULLUP);
  pinMode(PIN_BTN_UP, INPUT_PULLUP);
  pinMode(PIN_BTN_DOWN, INPUT_PULLUP);
  pinMode(PIN_BTN_SET, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  
  digitalWrite(PIN_LED, 1);

  lcd.init();
  lcd.backlight();

  if (!rtc.begin())
  {
    lcd.print("Loi RTC!");
    while (1);
  }

  if (!rtc.isrunning())
  {
    // Nếu RTC chưa chạy, set giờ theo thời gian biên dịch code
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  Buttons();
  
  if (isRing)
  {
    ringAlarm();
  }
  else
  {
    switch (currentMode) {
      case MODE_VIEW_TIME:
        displayRealTime();
        checkAlarm();
        break;
      case MODE_SET_ALARM_H:
      case MODE_SET_ALARM_M:
        displaySetAlarm();
        break;
      case MODE_SET_TIME_H:
      case MODE_SET_TIME_M:
        displaySetTime();
        break;
    }
  }
  delay(50);
}