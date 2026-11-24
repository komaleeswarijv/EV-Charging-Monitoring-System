
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Pin definitions
#define DHT_PIN 15
#define DHT_TYPE DHT22

#define VOLTAGE_PIN 34
#define CURRENT_PIN 35

#define BUZZER_PIN 19
#define SECURITY_PIN 18

// LCD and DHT objects
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHT_PIN, DHT_TYPE);

// Monitoring thresholds
const float MAX_CURRENT = 10.0;
const float THEFT_THRESHOLD = 1.0;
const float DRIVER_CHANGE_THRESHOLD = 2.5;

// Driver monitoring
float previousCurrent = 0;
bool firstReading = true;

void setup() {

  Serial.begin(115200);

  Wire.begin(21, 22);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(SECURITY_PIN, INPUT_PULLUP);

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

  // 1. Read simulated sensor values
  int voltageRaw = analogRead(VOLTAGE_PIN);
  int currentRaw = analogRead(CURRENT_PIN);

  float voltage = (voltageRaw / 4095.0) * 60.0;
  float current = (currentRaw / 4095.0) * 15.0;

  float temperature = dht.readTemperature();

  bool tempValid = !isnan(temperature);

  // 2. Security monitoring
  bool securityArmed =
    (digitalRead(SECURITY_PIN) == LOW);

  bool theftAlert =
    securityArmed && current > THEFT_THRESHOLD;

  // 3. Charging condition monitoring
  bool overcurrent = current > MAX_CURRENT;

  bool chargingFault =
    voltage <= 5.0 || current <= 0.2;

  bool lowVoltage =
    voltage > 5.0 && voltage < 40.0;

  bool slowCharging =
    voltage >= 40.0 &&
    voltage <= 55.0 &&
    current > 0.2 &&
    current < 2.0;

  bool chargingNormal =
    voltage >= 40.0 &&
    voltage <= 55.0 &&
    current >= 2.0 &&
    current <= MAX_CURRENT;

  // 4. Driver behaviour monitoring
  bool driverAlert = false;

  if (!firstReading) {
    driverAlert =
      abs(current - previousCurrent) >
      DRIVER_CHANGE_THRESHOLD;
  }

  previousCurrent = current;
  firstReading = false;

  // 5. Buzzer alert system
  // No buzzer for normal potentiometer adjustments

  if (theftAlert || overcurrent) {
    tone(BUZZER_PIN, 2000);
  }
  else if (lowVoltage) {
    tone(BUZZER_PIN, 1500, 300);
  }
  else {
    noTone(BUZZER_PIN);
  }

  // 6. LCD display - voltage and current
  lcd.setCursor(0, 0);
  lcd.print("V:");
  lcd.print(voltage, 1);
  lcd.print(" I:");
  lcd.print(current, 1);
  lcd.print("A ");

  // Clear second row
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);

  // Charging status display
  if (theftAlert) {
    lcd.print("THEFT ALERT!");
  }
  else if (overcurrent) {
    lcd.print("OVER CURRENT!");
  }
  else if (chargingFault) {
    lcd.print("NOT CHARGING");
  }
  else if (lowVoltage) {
    lcd.print("LOW VOLTAGE!");
  }
  else if (slowCharging) {
    lcd.print("SLOW CHARGING");
  }
  else if (chargingNormal) {
    lcd.print("NORMAL CHARGING");
  }
  else {
    lcd.print("CHECK CHARGING");
  }

  // 7. Serial Monitor output
  Serial.println("\n----- EV MONITOR -----");

  Serial.print("Voltage: ");
  Serial.print(voltage);
  Serial.println(" V");

  Serial.print("Current: ");
  Serial.print(current);
  Serial.println(" A");

  if (tempValid) {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C");
  }
  else {
    Serial.println("Temperature: Sensor Error");
  }

  Serial.print("Security: ");
  Serial.println(securityArmed ? "ARMED" : "DISARMED");

  Serial.print("Charging Status: ");

  if (overcurrent) {
    Serial.println("OVER CURRENT");
  }
  else if (chargingFault) {
    Serial.println("NOT CHARGING");
  }
  else if (lowVoltage) {
    Serial.println("LOW VOLTAGE");
  }
  else if (slowCharging) {
    Serial.println("SLOW CHARGING");
  }
  else if (chargingNormal) {
    Serial.println("NORMAL CHARGING");
  }
  else {
    Serial.println("CHECK CHARGING");
  }

  Serial.print("Driver Behaviour: ");
  Serial.println(driverAlert ? "LOAD ANOMALY" : "NORMAL");

  Serial.print("Anti-Theft: ");
  Serial.println(theftAlert ? "UNAUTHORIZED LOAD" : "SAFE");

  delay(2000);
}
