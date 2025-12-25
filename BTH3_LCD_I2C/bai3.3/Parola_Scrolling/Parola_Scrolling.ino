#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CS_PIN 10

MD_Parola parola = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

String inputText = "HELLO";
bool newMessage = false;

void setup() {
  Serial.begin(9600);
  parola.begin();
  parola.displayClear();
  parola.displayText((char*)inputText.c_str(), PA_CENTER, 80, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
}

void loop() {
  if (Serial.available() > 0) {
    inputText = Serial.readStringUntil('\n'); 
    inputText.trim(); 

    if (inputText.length() > 0) {
      newMessage = true;
    }
  }

  if (newMessage) {
    parola.displayClear();       
    parola.displayReset();      
    parola.displayText((char*)inputText.c_str(), PA_CENTER, 20, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    newMessage = false;
  }

  if (parola.displayAnimate()) {
    parola.displayReset();
  }
}
