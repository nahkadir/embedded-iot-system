// Task B — Use a single button with press-type detection 
// Short press → toggle LED & Long press (> 1.5 s) → play a buzzer tone
// Embedded IoT System Fall-2025

// Name: Rida Khan                 Reg#: 23-NTU-CS-1087
// Class/Section: BSCS-5B

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LED1       23         
#define BUTTON1    14
#define BUZZER_PIN 27     

#define PWM_CH      0
#define FREQ        2000
#define RESOLUTION  10

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool ledState = false;
unsigned long pressStart = 0;
bool buttonPressed = false;

void playBuzzerTone() {
  int melody[] = {
    330, 392, 330, 440, 494, 523, 392, 
    262, 330, 392, 262, 196, 262, 330
  };

  for (int i = 0; i < 8; i++) {
    ledcWriteTone(PWM_CH, melody[i]);
    delay(200);
  }
  ledcWrite(PWM_CH, 0);
}

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(BUTTON1, INPUT_PULLUP);

  // Buzzer setup
  ledcSetup(PWM_CH, FREQ, RESOLUTION);
  ledcAttachPin(BUZZER_PIN, PWM_CH);

  // OLED setup
  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 26);
  display.println("READY");
  display.display();
}

void loop() {
  // first time button is pressed, the start time is saved in pressStart
  if (digitalRead(BUTTON1) == LOW) {
    if (!buttonPressed) {
      buttonPressed = true;
      pressStart = millis();
      // millis() returns how many milliseconds the arduino has been running
    }
  } else if (buttonPressed) { // this code runs when button is released
    unsigned long pressDuration = millis() - pressStart;
    // press duration = current time - start time
    buttonPressed = false;

    if (pressDuration < 1500) {
      // short press, led toggled
      ledState = !ledState;
      digitalWrite(LED1, ledState ? HIGH : LOW);
      // Set LED1 to HIGH if ledState is true, otherwise set it to LOW

      display.clearDisplay();
      display.setCursor(10, 26);
      display.println(ledState ? "LED ON" : "LED OFF");
      display.display();

    } else {
      // long press, buzzer plays melody
      display.clearDisplay();
      display.setCursor(10, 26);
      display.println("BUZZER!");
      display.display();
      playBuzzerTone();
    }
  }
  delay(50);
}
