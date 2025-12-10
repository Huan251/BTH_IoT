#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 rtc;

void setup() {
  lcd.init();
  lcd.backlight();
  rtc.begin();

  if (!rtc.isrunning()) {
    lcd.clear();
    lcd.print("Dang cai RTC...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    delay(1500);
  }

  lcd.clear();
}

void loop() {
  static unsigned long lanCu = 0;
  unsigned long hienTai = millis();

  // Cập nhật mỗi 1 giây
  if (hienTai - lanCu >= 1000) {
    lanCu = hienTai;

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
    lcd.print("  "); // xóa ký tự thừa nếu có

    lcd.setCursor(0, 1);
    lcd.print("Date: ");
    if (now.day() < 10) lcd.print('0');
    lcd.print(now.day());
    lcd.print('/');
    if (now.month() < 10) lcd.print('0');
    lcd.print(now.month());
    lcd.print('/');
    lcd.print(now.year());
    lcd.print(" ");
  }
}
