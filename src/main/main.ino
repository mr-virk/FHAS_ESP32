// Blynk Metadata
#define BLYNK_TEMPLATE_ID "TMPL6eWdpZcmo"
#define BLYNK_TEMPLATE_NAME "ESP32"
#define BLYNK_AUTH_TOKEN "1sngMkuwDlrKcLqmvCFhKKK7gf_YUn1i"

// Libraries
#include <Wire.h>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <DHTesp.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Wifi Credentials
char ssid[] = "Wifi";
char pass[] = "pass";

// LCD Defination - I2C Address = 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin Definations
const int flamePin = 18;
const int gasPin = 19;
const int pirPin = 26;
const int relayPIR = 25; // Relay for PIR
const int relayWater = 23; // Relay for Water Pump
// const int buzzerPin  = 17;
const int waterPin = 34; // Water Level Sensor (Analog input)

// DHT Object with Pin Defination
DHTesp dht;
const int dhtPin = 16; // GPIO16 for DHT11

// Variables
int waterLevelPercent = 0;

// Main Intialization Function
void setup() {
  Serial.begin(115200);

  // LCD Default Screen
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Starting");

  // Sensors Intiallization
  dht.setup(dhtPin, DHTesp::DHT11);
  pinMode(flamePin, INPUT);
  pinMode(gasPin, INPUT);
  pinMode(pirPin, INPUT);
  pinMode(relayPIR, OUTPUT);
  pinMode(relayWater, OUTPUT);

  // Connect Blynk and WiFi
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // LCD Update
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi-Blynk Ready");
  delay(2000);
  lcd.clear();
}

// Main Primary Function
void loop() {
  Blynk.run();
  GatherDataFunction();
  delay(1000);
}

// PIR Relay control from Blynk App
BLYNK_WRITE(V5) {
  int value = param.asInt();
  digitalWrite(relayPIR, value);
}

// Water Relay control from App
BLYNK_WRITE(V6) {
  int value = param.asInt();
  digitalWrite(relayWater, value);
}

// ALert Tune Melody Function
void AlertTune() {
  int melody[] = { 1318, 1568, 1976, 1760, 1568, 1318, 1568, 1976, 1760, 1568 };
  int noteDurations[] = { 150, 150, 300, 300, 300, 150, 150, 300, 300, 300 };

  for (int i = 0; i < 10; i++) {
    ledcWriteTone(0, melody[i]);
    delay(noteDurations[i]);
    ledcWriteTone(0, 0);
    delay(50);
  }
}

// Main Secondry Function
void GatherDataFunction() {
  lcd.setCursor(0, 0);

  // DHT Sensor
  TempAndHumidity data = dht.getTempAndHumidity();
  if (!isnan(data.temperature) && !isnan(data.humidity)) {
    Serial.printf("[DHT OK] H: %.0f%%  T: %.0f\n", data.humidity, data.temperature);
    lcd.print("T:");
    lcd.print((int)data.temperature);
    lcd.print("C H:");
    lcd.print((int)data.humidity);

    // Send to Blynk
    Blynk.virtualWrite(V1, (int)data.temperature);
    Blynk.virtualWrite(V2, (int)data.humidity);
  } else {
    Serial.println("[DHT ERROR]");
    lcd.print("DHT ERROR     ");
  }

  // Flame Sensor
  int flameVal = digitalRead(flamePin);
  Serial.print("Flame: " + flameVal);

  if (flameVal == LOW) {
    ledcWriteTone(0, 2000); // Continuous beep
  } else {
    ledcWriteTone(0, 0);
  }

  // GAS Sensor
  int gasVal = digitalRead(gasPin);
  Serial.print("Gas: ");
  Serial.println(gasVal);
  if (gasVal == LOW) {
    AlertTune();
  }

  // PIR Sensor
  int motion = digitalRead(pirPin);
  Serial.print("Motion: " + motion);
  if (motion == HIGH) {
    digitalWrite(relayPIR, HIGH);
  } else {
    digitalWrite(relayPIR, LOW);
  }
  // Send to Blynk
  Blynk.virtualWrite(V4, motion);

  // Water Level
  int rawValue = analogRead(waterPin);
  waterLevelPercent = map(rawValue, 0, 4095, 0, 100);

  Serial.print("Water Level: ");
  Serial.print(waterLevelPercent);
  Serial.println("%");

  // Control water pump relay automatically based on the water level
  // Max Threashold is 25% to start the pump
  if (waterLevelPercent <= 25) {
    digitalWrite(relayWater, HIGH);  // Pump ON
  } else {
    digitalWrite(relayWater, LOW);  // Pump OFF
  }

  // Send to Blynk
  Blynk.virtualWrite(V3, waterLevelPercent);

  // Update LCD line 2
  lcd.setCursor(0, 1);
  lcd.print(" F:" + flameVal);
  lcd.print(" G:" + gasVal);
  lcd.print(" M:" + motion);
  lcd.print(" W:" + waterLevelPercent);
}