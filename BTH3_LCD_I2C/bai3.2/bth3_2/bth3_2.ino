#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

#define BTN_MODE 0
#define BTN_UP   1
#define BTN_DOWN 2
#define BTN_SET  3
#define LED_ALARM 6   // thay cho buzzer

LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 rtc;

enum ModeState {
  MODE_VIEW_TIME,
  MODE_SET_ALARM_H,
  MODE_SET_ALARM_M
};

ModeState mode = MODE_VIEW_TIME;

int alarmHour = 6, alarmMinute = 0;
bool alarmOn = false;
bool isRinging = false;

int tempHour, tempMinute;

unsigned long pressStart = 0;
bool holdDone = false;

void setup() {
  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_SET, INPUT_PULLUP);
  pinMode(LED_ALARM, OUTPUT);

  lcd.init();
  lcd.backlight();

  if (!rtc.begin()) {
    lcd.print("Loi RTC!");
    while (1);
  }
  if (!rtc.isrunning()) rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  lcd.clear();
}

void loop() {
  handleButtons();

  if (isRinging) {
    ringAlarm();
    return;
  }

  switch (mode) {
    case MODE_VIEW_TIME:
      displayRealTime();
      checkAlarmTrigger();
      break;

    case MODE_SET_ALARM_H:
    case MODE_SET_ALARM_M:
      displaySetAlarm();
      break;
  }

  delay(80);
}

void handleButtons() {
  int modeState = digitalRead(BTN_MODE);

  // --- Giữ nút MODE 2 giây để bật/tắt báo thức ---
  if (modeState == LOW) {
    if (pressStart == 0) pressStart = millis();
    if (millis() - pressStart > 2000 && !holdDone && mode == MODE_VIEW_TIME) {
      alarmOn = !alarmOn;
      holdDone = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(alarmOn ? "Bao thuc: ON " : "Bao thuc: OFF");
      delay(1000);
      lcd.clear();
    }
  } else {
    if (pressStart > 0) {
      if (!holdDone && millis() - pressStart < 1000) {
        // Nhấn ngắn: chuyển chế độ hoặc tắt chuông
        if (isRinging) stopAlarm();
        else changeMode();
      }
      pressStart = 0;
      holdDone = false;
    }
  }

  // Nút UP / DOWN / SET
  if (digitalRead(BTN_UP) == LOW) {
    handleUp();
    delay(200);
  }
  if (digitalRead(BTN_DOWN) == LOW) {
    handleDown();
    delay(200);
  }
  if (digitalRead(BTN_SET) == LOW) {
    handleSet();
    delay(300);
  }
}

// --- Chuyển chế độ ---
void changeMode() {
  lcd.clear();
  if (mode == MODE_VIEW_TIME) {
    mode = MODE_SET_ALARM_H;
    tempHour = alarmHour;
    tempMinute = alarmMinute;
  } else if (mode == MODE_SET_ALARM_H) {
    mode = MODE_SET_ALARM_M;
  } else {
    mode = MODE_VIEW_TIME;
  }
}

// --- Xử lý nút tăng/giảm ---
void handleUp() {
  if (mode == MODE_SET_ALARM_H) {
    tempHour = (tempHour + 1) % 24;
  } else if (mode == MODE_SET_ALARM_M) {
    tempMinute = (tempMinute + 1) % 60;
  }
}

void handleDown() {
  if (mode == MODE_SET_ALARM_H) {
    tempHour = (tempHour + 23) % 24;
  } else if (mode == MODE_SET_ALARM_M) {
    tempMinute = (tempMinute + 59) % 60;
  }
}

// --- Nút SET ---
void handleSet() {
  if (mode == MODE_SET_ALARM_H) {
    mode = MODE_SET_ALARM_M;
  } else if (mode == MODE_SET_ALARM_M) {
    alarmHour = tempHour;
    alarmMinute = tempMinute;
    alarmOn = true;
    mode = MODE_VIEW_TIME;
    lcd.clear();
    lcd.print("Da luu bao thuc!");
    delay(1000);
    lcd.clear();
  }
}

// --- Hiển thị thời gian thực ---
void displayRealTime() {
  DateTime now = rtc.now();

  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  if (now.hour() < 10) lcd.print('0');
  lcd.print(now.hour());
  lcd.print(':');
  if (now.minute() < 10) lcd.print('0');
  lcd.print(now.minute());
  lcd.print(':');
  if (now.second() < 10) lcd.print('0');
  lcd.print(now.second());

  lcd.setCursor(0, 1);
  lcd.print(alarmOn ? "ON " : "OFF");
  lcd.print(" Hen:");
  if (alarmHour < 10) lcd.print('0');
  lcd.print(alarmHour);
  lcd.print(':');
  if (alarmMinute < 10) lcd.print('0');
  lcd.print(alarmMinute);
}

// --- Hiển thị khi cài báo thức ---
void displaySetAlarm() {
  lcd.setCursor(0, 0);
  lcd.print("CAI BAO THUC");

  lcd.setCursor(0, 1);
  if (mode == MODE_SET_ALARM_H) lcd.print(">");
  else lcd.print(" ");
  if (tempHour < 10) lcd.print('0');
  lcd.print(tempHour);
  lcd.print(':');

  if (mode == MODE_SET_ALARM_M) lcd.print(">");
  else lcd.print(" ");
  if (tempMinute < 10) lcd.print('0');
  lcd.print(tempMinute);
}

// --- Kiểm tra đến giờ báo thức ---
void checkAlarmTrigger() {
  if (!alarmOn) return;

  DateTime now = rtc.now();
  if (now.hour() == alarmHour && now.minute() == alarmMinute && now.second() == 0) {
    isRinging = true;
  }
}

// --- Báo thức: LED chớp ---
void ringAlarm() {
  lcd.setCursor(0, 0);
  lcd.print("DEN GIO ROI!!!  ");
  lcd.setCursor(0, 1);
  lcd.print("Bam MODE de tat");

  digitalWrite(LED_ALARM, HIGH);
  delay(200);
  digitalWrite(LED_ALARM, LOW);
  delay(200);
}

// --- Tắt báo thức ---
void stopAlarm() {
  isRinging = false;
  digitalWrite(LED_ALARM, LOW);
  lcd.clear();
}
