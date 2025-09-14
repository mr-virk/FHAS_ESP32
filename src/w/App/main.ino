// Libraries
#include <Blynk.h>
#include <Wire.h>
#include <dhtnew.h>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6eWdpZcmo"
#define BLYNK_TEMPLATE_NAME "ESP32"

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Wifi";
char pass[] = "Wifipass";

// LCD object
LiquidCrystal_I2C lcd(0x3f, 16, 2); // Address, columns, rows 16x02

// Pin mapping
#define BUZZER_PIN 16
#define FLAME_PIN 17
#define GAS_PIN 18
#define RELAY1 19
#define RELAY2 20
#define PIR_PIN 21
#define WATER_PIN 22
DHTNEW dhtSensorPin(12); // Intialize DHT Sensor

// Sensor data
float temperature = 0, humidity = 0;
bool gasDetected = false;
bool waterDetected = false;
bool motionDetected = false;

// Variables
int MotionState; // Stores the current state of the PIR sensor
int currentHumidity;
int currentTemperature;
int flameDetected;
int gasDetected;

unsigned long uptimeStartTime = 0;  // Records the start time of the device

//For Backgruound Tasks
unsigned long previousTime = 0;
const unsigned long interval = 10;  // millisecond

// LCD Charaters Map
byte fire[8] = {B00000, B10000, B10100, B11101, B11111, B11111, B11111, B01110};
byte drop[8] = {B00000, B00100, B01110, B11111, B11111, B11111, B01110, B00000};
byte temp[8] = {B01110, B01010, B01010, B01010, B01010, B10001, B10001, B01110};

// Function Prototypes


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
  pinMode(BUZZER_PIN, OUTPUT);
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
  uptimeStartTime = millis(); // Record the start time of the device
  lcdpreview();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass); //Blynk
  delay(10);
}

void loop()
{
  unsigned long currentTime = millis();

  CheckUptime();
  Blynk.run();
}

BLYNK_CONNECTED() {
  // Restore switch states from Blynk app on connection
  Blynk.syncVirtual(V1, V2, V3, V4, V5, V6); //Virtual Pins
}

void CheckUptime()
{
  unsigned long currentMillis = millis();
  unsigned long uptimeSeconds = (currentMillis - uptimeStartTime) / 1000;

  sensorsReading();
}

void sensorsReading(){
  readDHT();
  detectMotion();
  detectGas();
  detectFlame();
  lcdDefault();
  delay(10);
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
    tone(BUZZER_PIN, 1000);
    delay(500);

    // Turn backlight on
    lcd.backlight();
    noTone(BUZZER_PIN);
    delay(500);
  }
  lcd.clear();
}

void lcdDefault()
{
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

    Blynk.virtualWrite(V1, currentHumidity);
    Blynk.virtualWrite(V2, currentTemperature);
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
      tone(BUZZER_PIN, 1000);
      delay(300);

      lcd.backlight();
      noTone(BUZZER_PIN);
      delay(300);
    }
    lcd.clear();
  }
}

void detectGas() {
  gasDetected = digitalRead(GAS_PIN);  //sensor has inverted logic
  if (gasDetected == HIGH) {
    Blynk.logEvent("gas_leakage", "Gas Leakage has been Detected!");
    alertFunction();
  }
}

void detectFlame() {
  flameDetection= digitalRead(FLAME_PIN);
  //Serial.println(flameValue);
  if (flameDetection == HIGH) {
    Blynk.logEvent("fire_alarm", "Fire has been Detected!");
    alertFunction();
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
  lcd.print((fireDetected || gasDetected) ? " Fire Detected! " : " Gas Detected! ");

  // Backlight control
  for (uint8_t i = 0; i < 30; i++) {
    lcd.noBacklight();
    tone(BUZZER_PIN, 1000);
    delay(500);

    lcd.backlight();
    noTone(BUZZER_PIN);
    delay(500);
  }
  lcd.clear();
}