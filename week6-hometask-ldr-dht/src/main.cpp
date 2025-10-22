// Temperature & Humidity Sensor + Light Dependent Resistor

// Rida Khan
// 23-NTU-CS-FL-1087
// BSCS-5B

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// --- Pin configuration ---
#define LDR_PIN 34
#define DHTPIN 14
#define DHTTYPE DHT11

#define SDA_PIN 21
#define SCL_PIN 22

// --- OLED setup ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- DHT setup ---
DHT dht(DHTPIN, DHTTYPE);

// --- Setup ---
void setup() {
  Serial.begin(115200);
  Serial.println("ESP32: LDR + DHT22 + OLED");

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();

  dht.begin();
  delay(1000);
}

// --- Loop ---
void loop() {
  // --- Read DHT22 ---
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // --- Read LDR ---
  int adcValue = analogRead(LDR_PIN);
  float voltage = (adcValue / 4095.0) * 3.3;

  // --- Check DHT errors ---
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT22 read failed!");
    return;
  }

  // --- Serial output ---
  Serial.printf("Temp: %.1f°C | Humidity: %.1f%% | LDR: %d | Voltage: %.2fV\n",
                temperature, humidity, adcValue, voltage);

  // --- OLED display ---
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("🌡  DHT22 + LDR Data");

  display.setCursor(0, 16);
  display.print("Temp: ");
  display.print(temperature);
  display.println(" C");

  display.setCursor(0, 28);
  display.print("Humidity: ");
  display.print(humidity);
  display.println(" %");

  display.setCursor(0, 40);
  display.print("LDR ADC: ");
  display.println(adcValue);

  display.setCursor(0, 52);
  display.print("Voltage: ");
  display.print(voltage, 2);
  display.println(" V");

  display.display();

  delay(2000);
}