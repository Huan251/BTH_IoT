#define BUTTON_PIN 4
#define LED_PIN 10

bool configMode = false;       // false: binh thuong ; true: cau hinh
bool lastButtonState = HIGH;   // Khong nhan : HIGH, nhan : LOW
unsigned long pressStart = 0;
int baudOption = 0;            // 0: 9600, 1: 115200

void nhayLed(int soLan, int delayMs) {
  for (int i = 0; i < soLan; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(delayMs);
    digitalWrite(LED_PIN, LOW);
    delay(delayMs);
  }
}

void binhthuong(int baud) {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(baud);
  Serial.println("He thong dang o che do binh thuong!");
  digitalWrite(LED_PIN, LOW);
}

void cauhinh() {
  Serial.println(">>> Vao che do cau hinh UART <<<");
  Serial.println("Da chon toc do 9600 bps.");
  nhayLed(5, 100);
  digitalWrite(LED_PIN, LOW); 
}

void setup() {
  binhthuong(9600);
}

void loop() {
  bool buttonState = digitalRead(BUTTON_PIN);

  // Ghi lai thoi gian nhan
  if (buttonState == LOW && lastButtonState == HIGH) {
    pressStart = millis();
  }

  // Phat hien nhan giu >3s
  if (buttonState == LOW && (millis() - pressStart > 3000)) {
    if (!configMode) {
      configMode = true;
      cauhinh();
    } else {
      int baud = (baudOption == 0) ? 9600 : 115200;
      binhthuong(baud);
      Serial.println("He thong tro ve che do binh thuong.");
      configMode = false;
    }

    while (digitalRead(BUTTON_PIN) == LOW);
    delay(300);
  }

  if (configMode) {
    // Nhan nut de doi toc do
    if (buttonState == LOW && lastButtonState == HIGH) {
      if (baudOption == 0) baudOption = 1;
      else baudOption = 0;

      nhayLed(1, 100);
      if (baudOption == 0) {
        Serial.begin(9600);
        Serial.println("Da chon toc do 9600 bps.");
      } else {
        Serial.begin(115200);
        Serial.println("Da chon toc do 115200 bps.");
      }

      delay(300);
    }
  }

  else {
    Serial.println("He thong dang hoat dong");
    delay(500);
  }

  lastButtonState = buttonState;
}
