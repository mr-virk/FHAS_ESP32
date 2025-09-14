// Libraries
#include "test.h"
#include <Wire.h>
#include <dhtnew.h>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Wifi";
char pass[] = "Wifipass";

// LCD object
LiquidCrystal_I2C lcd(0x3f, 16, 2); // Address, columns, rows 16x02

// Pin mapping
#define BUZZER 16
#define FLAME_PIN 17
#define GAS_PIN 18
#define RELAY1 19
#define RELAY2 20
#define PIR_PIN 21
#define WATER_PIN 22
DHTNEW dhtSensorPin(12); // Intialize DHT Sensor

// Timing
unsigned long lastDHT = 0, lastLCD = 0;

// Shared variables
volatile bool flameDetected = false;
volatile bool gasDetected = false;
volatile bool motionDetected = false;
volatile bool waterDetected = false;

// Sensor data
float temperature = 0, humidity = 0;
bool flameDetected = false;
bool gasDetected = false;
bool waterDetected = false;
bool motionDetected = false;

// Variables
int MotionState = LOW; // Stores the current state of the PIR sensor
int currentHumidity;
int currentTemperature;

// LCD Charaters Map
byte fire[8] = {B00000, B10000, B10100, B11101, B11111, B11111, B11111, B01110};
byte drop[8] = {B00000, B00100, B01110, B11111, B11111, B11111, B01110, B00000};
byte temp[8] = {B01110, B01010, B01010, B01010, B01010, B10001, B10001, B01110};

// Function Prototypes
void readSensors();
void readDHT();
void updateLCD();
void handleAlarms();

void setup()
{
  Serial.begin(115200); // Serial rate

  // Initialize pins
  pinMode(FLAME_PIN, INPUT);
  pinMode(GAS_PIN, INPUT);
  pinMode(WATER_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(RELAY1, HIGH);
  digitalWrite(RELAY2, HIGH);

  // Initialize LCD
  lcd.createChar(0, fire);
  lcd.createChar(1, drop);
  lcd.createChar(2, temp);

  lcd.init();      // lcd start
  lcd.backlight(); // backlight on
  lcd.clear();     // lcd clear

  lcd.setCursor(0, 0);
  lcd.print(" Connecting to ");
  lcd.setCursor(0, 1);
  lcd.print(" Wifi...");

  Serial.println("Smart Home System Starting...");

  delay(10);

  uptimeStartTime = millis(); // Record the start time of the device
  lcdpreview();
}

void loop()
{
  unsigned long currentTime = millis();

  CheckUptime();

  RunFunctions(); // Call the global millis function

  // Call the menu function that handles user input and background tasks

  delay(10);
}

void CheckUptime()
{
  unsigned long currentMillis = millis();
  unsigned long uptimeSeconds = (currentMillis - uptimeStartTime) / 1000;
}

void lcdpreview()
{
  lcd.setCursor(0, 0);
  lcd.print("Home Automation");
  lcd.setCursor(0, 1);
  lcd.print("System");

  lcd.setCursor(6, 1);
  lcd.write(0);
  lcd.setCursor(7, 1);
  lcd.write(1);
  lcd.setCursor(8, 1);
  lcd.write(2);

  // Backlight control
  for (uint8_t i = 0; i < 3; i++)
  {
    // Turn backlight off
    lcd.noBacklight();
    tone(buzzer, 1000);
    delay(500);

    // Turn backlight on
    lcd.backlight();
    noTone(buzzer);
    delay(500);
  }
  lcd.clear();
}

void lcdDefault()
{

  if (millis() - lastLCD > 1000)
  {
    lastLCD = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(1);
    lcd.setCursor(2, 0);
    lcd.print("Humi. (%):"); // 0-9

    lcd.setCursor(0, 1);
    lcd.write(2);
    lcd.setCursor(2, 1);
    lcd.print("Temp. (C):"); // 0-10

    lcd.setCursor(12, 0);
    lcd.print(currentHumidity);
    lcd.setCursor(12, 1);
    lcd.print(currentTemperature);
  }
}

void readDHT()
{
  // READ DATA
  int chk = dhtSensorPin.read();

  String dhtErrorGen;

  if (chk != DHTLIB_WAITING_FOR_READ)
  {
    switch (chk)
    {
    case DHTLIB_OK:
      dhtErrorGen = chk;
      break;
    case DHTLIB_ERROR_CHECKSUM:
      dhtErrorGen = chk;
      break;
    case DHTLIB_ERROR_TIMEOUT_A:
      dhtErrorGen = chk;
      break;
    case DHTLIB_ERROR_TIMEOUT_B:
      dhtErrorGen = chk;
      break;
    case DHTLIB_ERROR_TIMEOUT_C:
      dhtErrorGen = chk;
      break;
    case DHTLIB_ERROR_TIMEOUT_D:
      dhtErrorGen = chk;
      break;
    case DHTLIB_ERROR_SENSOR_NOT_READY:
      dhtErrorGen = chk;
      break;
    case DHTLIB_ERROR_BIT_SHIFT:
      dhtErrorGen = chk;
      break;
    default:
      dhtErrorGen = chk;
      break;
    }

    currentHumidity = dhtSensorPin.getHumidity();
    currentTemperature = dhtSensorPin.getTemperature();
  }
}

void detectMotion()
{
  motionDetected = digitalRead(PIR_PIN);
  if (motionDetected == HIGH)
  {
    // Blynk.logEvent("motion_detected", "Alert: Motion has been Detected!");
    lcd.setCursor(0, 0);
    lcd.write(1);
    lcd.setCursor(2, 0);
    lcd.print("  WARNING! ");
    lcd.setCursor(15, 0);
    lcd.write(1);
    lcd.setCursor(0, 1);
    lcd.print("Motion Detected!");

    // Backlight control
    for (uint8_t i = 0; i < 10; i++)
    {
      lcd.noBacklight();
      tone(buzzer, 1000);
      delay(300);

      lcd.backlight();
      noTone(buzzer);
      delay(300);
    }
    lcd.clear();
  }
}

void detectGas() {
  int gasDetected = digitalRead(GAS_PIN);  //sensor has inverted logic
  if (gasDetected == HIGH) {
    // Blynk.logEvent("gas_leakage", "Gas Leakage has been Detected!");
    alertFunction();
  }
}

void detectFlame() {
  int flameDetected = digitalRead(FLAME_PIN);
  //Serial.println(flameValue);
  if (flameDetected == HIGH) {
    // Blynk.logEvent("fire_alarm", "Fire has been Detected!");
    alertFunction();

    lcd_blynk.clear();
    lcd_blynk.print(0, 0, "FLAME DETECTED!!!");
  }
}

void alertFunction() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(0);
  lcd.setCursor(2, 0);
  lcd.print("  WARNING! ");
  lcd.setCursor(15, 0);
  lcd.write(0);
  lcd.setCursor(0, 1);
  lcd.print(" Fire Detected! ");

  lcd_blynk.clear();
  lcd_blynk.print(2, 0, "  WARNING! ");
  lcd_blynk.print(0, 1, " Fire Detected! ");


  // Backlight control
  for (uint8_t i = 0; i < 30; i++) {
    lcd.noBacklight();
    tone(buzzer, 1000);
    delay(500);

    lcd.backlight();
    noTone(buzzer);
    delay(500);
  }
  lcd.clear();
}