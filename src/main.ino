//Libraries
#include <Wire.h>
#include <dhtnew.h>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Wifi";
char pass[] = "Wifipass";

// LCD object
LiquidCrystal_I2C lcd(0x3f, 16, 2);  //Address, columns, rows 16x02

// Pin assignments
DHTNEW dhtSensorPin();  //Intialize DHT Sensor
#define IRSensorPin ;
const int buzzerPin = ;
const int flamePin = ;
const int gasPin = ;
const int relay1Pin = ;
const int relay2Pin = ;
const int pirPin = ;

//Variables
int MotionState = LOW;  // Stores the current state of the PIR sensor
int currentHumidity;
int currentTemperature;

//Flags
bool relay1Status = false;
bool relay2Status = false;
bool motionDetected = false;  // Flag to indicate motion detection

//LCD Charaters Map
byte fire[8] = { B00000, B10000, B10100, B11101, B11111, B11111, B11111, B01110 };
byte drop[8] = { B00000, B00100, B01110, B11111, B11111, B11111, B01110, B00000 };
byte temp[8] = { B01110, B01010, B01010, B01010, B01010, B10001, B10001, B01110 };

void setup() {
  Serial.begin(115200); //Serial rate

  // Initialize LCD
  lcd.init(); //lcd start
  lcd.backlight(); //backlight on
  lcd.clear(); //lcd clear

  lcd.createChar(0, fire);
  lcd.createChar(1, drop);
  lcd.createChar(2, temp);

  lcd.setCursor(0, 0);
  lcd.print(" Connecting to ");
  lcd.setCursor(0, 1);
  lcd.print(" Wifi...");

  delay(10);

  pinMode(flamePin, INPUT);
  pinMode(gasPIn, INPUT);
  pinMode(pirPin, INPUT);

  pinMode(buzzer, OUTPUT);
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  

  uptimeStartTime = millis();  // Record the start time of the device
  lcdpreview();
}

void loop() {
  unsigned long currentTime = millis();

  CheckUptime();

  RunFunctions();  // Call the global millis function

  // Call the menu function that handles user input and background tasks

  Blynk.run();

  delay(10);
}

void CheckUptime() {
  unsigned long currentMillis = millis();
  unsigned long uptimeSeconds = (currentMillis - uptimeStartTime) / 1000;
}

void RunFunctions() {
  unsigned long currentTime = millis();
  if (currentTime - previousTime >= interval) {
    previousTime = currentTime;

    // Code to execute every 3 second
    ldrSensor();
    IRSwitchFunction();
    dhtSensorChk();
    flameSensor();
    gasSensor();
    MotionDetection();
    lcdDefault();
  }
}

void lcdpreview() {
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
  for (uint8_t i = 0; i < 3; i++) {
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

void lcdDefault() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(1);
  lcd.setCursor(2, 0);
  lcd.print("Humi. (%):");  //0-9

  lcd.setCursor(0, 1);
  lcd.write(2);
  lcd.setCursor(2, 1);
  lcd.print("Temp. (C):");  //0-10

  lcd.setCursor(12, 0);
  lcd.print(currentHumidity);
  lcd.setCursor(12, 1);
  lcd.print(currentTemperature);
}

void MotionDetection() {
  MotionState = digitalRead(-);
  if (MotionDetected == true) {
    if (MotionDetected == HIGH) {
      MotionDetected();
    }
  }
}

void MotionDetected() {
  digitalwrite(pirPin, HIGH);
}

void dhtSensorChk() {
  // READ DATA
  int chk = dhtSensorPin.read();

  String dhtErrorGen;

  if (chk != DHTLIB_WAITING_FOR_READ) {
    switch (chk) {
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