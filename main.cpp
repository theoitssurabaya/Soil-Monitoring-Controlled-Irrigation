/********************************************************************
 * Smart Plant Watering Node (ESP32)
 * Tujuan: Membaca sensor dan mengirim paket JSON ke Raspberry Pi via Serial2.
 * Arsitektur: ESP32 = SENSOR NODE SAJA. RPi = GATEWAY & AKTUATOR.
 ********************************************************************/

#include <Arduino.h>
#include <DHT.h>

// ---------- Definisi Pin ----------
// Komunikasi Serial ke Raspberry Pi (UART2)
#define RPI_RX_PIN 16
#define RPI_TX_PIN 17

// Pin Sensor
#define PIN_SOIL 35
#define PIN_LDR 32
#define DHTPIN 14
#define DHTTYPE DHT22

// ---------- Objek Global ----------
DHT dht(DHTPIN, DHTTYPE);

// ---------- Timer ----------
unsigned long lastSend = 0;
const unsigned long sendInterval = 5000;

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RPI_RX_PIN, RPI_TX_PIN);

  dht.begin();

  pinMode(PIN_SOIL, INPUT);
  pinMode(PIN_LDR, INPUT);

  Serial.println("=== ESP32 Sensor Node Aktif ===");
  Serial.println("Fungsi: Baca Sensor & Kirim Data via Serial2 ke RPi.");
  delay(2000);
  Serial.println("Mulai pengiriman data sensor...");
}

void loop() {
  unsigned long now = millis();

  if (now - lastSend >= sendInterval) {
    lastSend = now;

    int soilRaw = analogRead(PIN_SOIL);
    int ldrRaw = analogRead(PIN_LDR);
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || humidity < 0) humidity = -1.0;
    if (isnan(temperature) || temperature < 0) temperature = -1.0;

    String payload = "START";
    payload += "{";
    payload += "\"soil\":" + String(soilRaw);
    payload += ",\"temp\":" + String(temperature, 1);
    payload += ",\"hum\":" + String(humidity, 1);
    payload += ",\"light\":" + String(ldrRaw);
    payload += "}";
    payload += "END";

    Serial2.println(payload);
    Serial2.flush();

    Serial.println("[SEND] " + payload);
  }

  // Tidak ada penerimaan command dari RPi
}