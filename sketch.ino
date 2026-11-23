
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Pin definitions
#define DHT_PIN 15
#define DHT_TYPE DHT22

#define VOLTAGE_PIN 34
#define CURRENT_PIN 35

// LCD and DHT objects
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHT_PIN, DHT_TYPE);

// Monitoring threshold
const float MAX_CURRENT = 10.0;

void setup() {
  Serial.begin(115200);

  Wire.begin(21, 22);

  lcd.init();
  lcd.backlight();

  dht.begin();

  lcd.setCursor(0, 0);
  lcd.print("EV Monitoring");
  lcd.setCursor(0, 1);
  lcd.print("System Starting");

  delay(2000);
  lcd.clear();
}

void loop() {
  // Read simulated sensor values
  int voltageRaw = analogRead(VOLTAGE_PIN);
  int currentRaw = analogRead(CURRENT_PIN);

  float voltage = (voltageRaw / 4095.0) * 60.0;
  float current = (currentRaw / 4095.0) * 15.0;

  float temperature = dht.readTemperature();

  // Handle invalid temperature readings
  if (isnan(temperature)) {
    Serial.println("DHT22 reading error");
    delay(2000);
    return;
  }

  // Determine charging status
  bool overcurrent = current > MAX_CURRENT;

  // Display on LCD
  lcd.setCursor(0, 0);
  lcd.print("V:");
  lcd.print(voltage, 1);
  lcd.print(" I:");
  lcd.print(current, 1);
  lcd.print("A  ");

  lcd.setCursor(0, 1);

  if (overcurrent) {
    lcd.print("OVER CURRENT!   ");
  } else {
    lcd.print("Temp:");
    lcd.print(temperature, 1);
    lcd.print((char)223);
    lcd.print("C       ");
  }

  // Serial Monitor output
  Serial.println("----- EV MONITOR -----");
  Serial.print("Voltage: ");
  Serial.print(voltage);
  Serial.println(" V");

  Serial.print("Current: ");
  Serial.print(current);
  Serial.println(" A");

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");

  Serial.print("Charging Status: ");
  Serial.println(overcurrent ? "ABNORMAL" : "NORMAL");

  delay(2000);
}
