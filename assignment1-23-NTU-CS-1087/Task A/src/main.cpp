// Task A: Use one button to cycle through LED modes & Use the second button to reset to OFF.
// Embedded IoT System Fall-2025

// Name: Rida Khan                 Reg#: 23-NTU-CS-1087
// Class/Section: BSCS-5B

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LED1       23         
#define LED2       19          
#define LED3       17          
#define BUTTON1    14          // Mode change button
#define BUTTON2    13          // Reset button

#define DEBOUNCE_MS   50
#define DEBOUNCE_US   (DEBOUNCE_MS * 1000UL)

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

hw_timer_t* debounceTimer = nullptr; 
volatile bool debounceActive = false;   
volatile int modeCount = 0; 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Debounce timer ISR 
void IRAM_ATTR onDebounceTimer() {
  if (digitalRead(BUTTON1) == LOW) {
    modeCount++;
    if (modeCount > 3) modeCount = 0;
  }

  if (digitalRead(BUTTON2) == LOW) {
    modeCount = 0;
  }

  debounceActive = false; // allows more button presses
}

// IRAM_ATTR marked for fast execution from RAM
// Button1 interrupt ISR 
void IRAM_ATTR onButtonISR() {
  if (!debounceActive) { // press ignored if timer is already running
    debounceActive = true; // else set to true (timeer running now)
    timerWrite(debounceTimer, 0); // stopwatch reset to 0
    timerAlarmWrite(debounceTimer, DEBOUNCE_US, false); // time set till 50ms
    timerAlarmEnable(debounceTimer); // and the timer is started
  } // after 50ms, onDebounceTimer will be called
}

// Display
void showMode(const char* text) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 26);
  display.println(text);
  display.display();
}

void setup() {
  // PWM setup
  ledcSetup(2, 5000, 8);
  ledcAttachPin(LED3, 2);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);

  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  // when each button is pressed, onButtonISR is triggered
  attachInterrupt(BUTTON1, onButtonISR, FALLING);
  attachInterrupt(BUTTON2, onButtonISR, FALLING);

  // timer created which uses hardware timer 3 and calls onDebounceTimer when time finishes
  debounceTimer = timerBegin(3, 80, true);
  timerAttachInterrupt(debounceTimer, &onDebounceTimer, true);

  // OLED setup
  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    for (;;);
  }

  display.clearDisplay();
  showMode("ALL OFF");
}

void loop() {
  switch (modeCount) {
    case 0:  // BOTH OFF
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
      ledcWrite(2, 0);
      showMode("BOTH OFF");
      break;

    case 1:  // Alternate Blink
      showMode("ALTERNATE");
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, HIGH);
      ledcWrite(2, 0);
      delay(400);
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2,LOW);
      ledcWrite(2, 0);
      delay(400);
      break;

    case 2:  // BOTH ON
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      ledcWrite(2, 0);
      showMode("BOTH ON");
      break;

    case 3:  // PWM Fade
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
      showMode("PWM FADE");
      for (int d = 0; d <= 255; d = d + 5) { 
        ledcWrite(2, d);
        delay(10); 
      }
      for (int d = 255; d >= 0; d = d - 5) { 
        ledcWrite(2, d);
        delay(10);
      }
      break;
  }

  delay(50);
}
