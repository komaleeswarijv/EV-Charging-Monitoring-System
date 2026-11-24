
#define BLYNK_TEMPLATE_ID "TMPL3mV9B5mEe"
#define BLYNK_TEMPLATE_NAME "EV Charging Monitoring System"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
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

BlynkTimer timer;

// Monitoring thresholds
const float MAX_CURRENT = 10.0;
const float THEFT_THRESHOLD = 1.0;
const float DRIVER_CHANGE_THRESHOLD = 2.5;

// Driver monitoring
float previousCurrent = 0;
bool firstReading = true;

void monitorEV() {

  // 1. Read sensors
  int voltageRaw = analogRead(VOLTAGE_PIN);
  int currentRaw = analogRead(CURRENT_PIN);

  float voltage = (voltageRaw / 4095.0) * 60.0;
  float current = (currentRaw / 4095.0) * 15.0;
  float temperature = dht.readTemperature();

  bool tempValid = !isnan(temperature);

  // 2. Security monitoring
  bool securityArmed = (digitalRead(SECURITY_PIN) == LOW);

  bool theftAlert =
    securityArmed && current > THEFT_THRESHOLD;

  // 3. Charging monitoring
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

  // 5. Determine charging status
  String chargingStatus;

  if (theftAlert)
    chargingStatus = "THEFT ALERT";
  else if (overcurrent)
    chargingStatus = "OVER CURRENT";
  else if (chargingFault)
    chargingStatus = "NOT CHARGING";
  else if (lowVoltage)
    chargingStatus = "LOW VOLTAGE";
  else if (slowCharging)
    chargingStatus = "SLOW CHARGING";
  else if (chargingNormal)
    chargingStatus = "NORMAL CHARGING";
  else
    chargingStatus = "CHECK CHARGING";

  // 6. Buzzer
  if (theftAlert || overcurrent) {
    tone(BUZZER_PIN, 2000);
  }
  else if (lowVoltage) {
    tone(BUZZER_PIN, 1500, 300);
  }
  else {
    noTone(BUZZER_PIN);
  }

  // 7. LCD display
  lcd.setCursor(0, 0);
  lcd.print("V:");
  lcd.print(voltage, 1);
  lcd.print(" I:");
  lcd.print(current, 1);
  lcd.print("A ");

  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(chargingStatus);

  // 8. Serial Monitor
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
  Serial.println(chargingStatus);

  Serial.print("Driver Behaviour: ");
  Serial.println(driverAlert ? "LOAD ANOMALY" : "NORMAL");

  Serial.print("Anti-Theft: ");
  Serial.println(theftAlert ? "UNAUTHORIZED LOAD" : "SAFE");

  // 9. Send data to Blynk
  Blynk.virtualWrite(V0, voltage);
  Blynk.virtualWrite(V1, current);

  if (tempValid) {
    Blynk.virtualWrite(V2, temperature);
  }

  Blynk.virtualWrite(V3, chargingStatus);

  String securityStatus;

  if (theftAlert)
    securityStatus = "THEFT ALERT";
  else if (securityArmed)
    securityStatus = "ARMED";
  else
    securityStatus = "DISARMED";

  Blynk.virtualWrite(V4, securityStatus);
}

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

  // Connect to Blynk using Wokwi WiFi
  Serial.println("Connecting to Blynk...");

  WiFi.mode(WIFI_STA);
  WiFi.begin("Wokwi-GUEST", "", 6);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.println(WiFi.localIP());

  Blynk.config(BLYNK_AUTH_TOKEN);

  if (Blynk.connect(10000)) {
    Serial.println("Blynk Connected!");
  } else {
    Serial.println("Blynk Connection Failed!");
  }

  Serial.println("Blynk Connected!");

  // Monitor and upload every 2 seconds
  timer.setInterval(2000L, monitorEV);
}

void loop() {

  Blynk.run();
  timer.run();

}
