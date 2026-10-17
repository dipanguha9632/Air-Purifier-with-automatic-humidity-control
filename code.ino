#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int FAN1_PIN = 3;
const int FAN2_PIN = 4;
const int PELTIER_PIN = 5;
const int MIST_PIN = 6;

const int MQ2_PIN = A0;
const int MQ_PIN = A1;
const int MQ135_PIN = A2;

const float HUMIDITY_HIGH = 70.0;
const float HUMIDITY_LOW = 40.0;

const bool ACTIVE_HIGH = true;

unsigned long lastPageChange = 0;
const unsigned long PAGE_INTERVAL = 3000;

int page = 0;

float humidity = 0;
float temperature = 0;

int mq2Value = 0;
int mqValue = 0;
int mq135Value = 0;

bool peltierOn = false;
bool mistOn = false;

void setOutput(int pin, bool turnOn) {
  if (ACTIVE_HIGH) {
    digitalWrite(pin, turnOn ? HIGH : LOW);
  } else {
    digitalWrite(pin, turnOn ? LOW : HIGH);
  }
}

void setup() {
  pinMode(FAN1_PIN, OUTPUT);
  pinMode(FAN2_PIN, OUTPUT);
  pinMode(PELTIER_PIN, OUTPUT);
  pinMode(MIST_PIN, OUTPUT);

  setOutput(FAN1_PIN, false);
  setOutput(FAN2_PIN, false);
  setOutput(PELTIER_PIN, false);
  setOutput(MIST_PIN, false);

  dht.begin();

  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("AIR PURIFIER");
  lcd.setCursor(0, 1);
  lcd.print("System Starting");

  delay(2000);

  setOutput(FAN1_PIN, true);
  setOutput(FAN2_PIN, true);
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (!isnan(h)) {
    humidity = h;
  }

  if (!isnan(t)) {
    temperature = t;
  }

  mq2Value = analogRead(MQ2_PIN);
  mqValue = analogRead(MQ_PIN);
  mq135Value = analogRead(MQ135_PIN);

  if (isnan(h)) {
    peltierOn = false;
    mistOn = false;
  } else {
    if (humidity > HUMIDITY_HIGH) {
      peltierOn = true;
      mistOn = false;
    } else if (humidity < HUMIDITY_LOW) {
      peltierOn = false;
      mistOn = true;
    } else {
      peltierOn = false;
      mistOn = false;
    }
  }

  setOutput(FAN1_PIN, true);
  setOutput(FAN2_PIN, true);
  setOutput(PELTIER_PIN, peltierOn);
  setOutput(MIST_PIN, mistOn);

  if (millis() - lastPageChange >= PAGE_INTERVAL) {
    lastPageChange = millis();
    page = (page + 1) % 3;
    lcd.clear();
  }

  if (page == 0) {
    lcd.setCursor(0, 0);
    lcd.print("H:");

    if (isnan(h)) {
      lcd.print("Sensor Error");
    } else {
      lcd.print(humidity, 1);
      lcd.print("% T:");
      lcd.print(temperature, 0);
      lcd.print("C");
    }

    lcd.setCursor(0, 1);
    lcd.print("P:");
    lcd.print(peltierOn ? "ON " : "OFF");
    lcd.print(" M:");
    lcd.print(mistOn ? "ON " : "OFF");
  } else if (page == 1) {
    lcd.setCursor(0, 0);
    lcd.print("MQ2:");
    lcd.print(mq2Value);
    lcd.print(" MQ:");
    lcd.print(mqValue);

    lcd.setCursor(0, 1);
    lcd.print("Fans: ON  ON");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("MQ135:");
    lcd.print(mq135Value);

    lcd.setCursor(0, 1);
    lcd.print("Air Quality Raw");
  }

  delay(1000);
}