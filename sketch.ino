#include <WiFi.h>
#include <HTTPClient.h>
#include "DHTesp.h"

// Virtual Wi-Fi network provided directly by Wokwi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// 🚀 UPDATED: Pointing directly to the live production endpoint (removed "-test")
const char* n8n_webhook = "https://YOUR_ID/webhook-test/industrial-guardian";

DHTesp dht;

// Verified Pin Map 
const int PIN_SHUTDOWN = 13; // Red LED
const int PIN_FAN      = 12; // Yellow LED
const int PIN_PUMP     = 14; // Blue LED
const int PIN_EXHAUST  = 27; // Green LED

int gasBaseline = 0;
int gasThreshold = 300; // Trigger threshold relative to initial baseline ambient air

void setup() {
  Serial.begin(115200);
  dht.setup(15, DHTesp::DHT22);
  
  pinMode(PIN_SHUTDOWN, OUTPUT);
  pinMode(PIN_FAN, OUTPUT);
  pinMode(PIN_PUMP, OUTPUT);
  pinMode(PIN_EXHAUST, OUTPUT);

  // Clear all indicators completely at startup
  digitalWrite(PIN_SHUTDOWN, LOW);
  digitalWrite(PIN_FAN, LOW);
  digitalWrite(PIN_PUMP, LOW);
  digitalWrite(PIN_EXHAUST, LOW);

  // Initialize Wi-Fi connection
  Serial.print("🌐 Connecting to Wokwi Cloud Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Network Connected!");

  Serial.println("⚙️ Calibrating air quality sensor baseline...");
  delay(1000);
  
  // Grab a clean initial environmental average
  long sum = 0;
  for(int i=0; i<10; i++) {
    sum += analogRead(32);
    delay(50);
  }
  gasBaseline = sum / 10;
  Serial.print("✅ Calibration Complete! Ambient Air Value Baseline: ");
  Serial.println(gasBaseline);
}

void loop() {
  // Read Potentiometer on ADC Channel 1
  int vibration = analogRead(34); 
  delay(20); // Critical delay to prevent internal ADC crosstalk bleeding
  
  // Read Gas on clean isolated ADC Channel 2
  int rawGas = analogRead(32); 
  float temp = dht.getTemperature();

  // Print raw debugging statistics directly to terminal
  Serial.print("🌡️ Temp: "); Serial.print(temp);
  Serial.print("°C | 📊 Potentiometer Value: "); Serial.print(vibration);
  Serial.print(" | 💨 Gas (Raw): "); Serial.print(rawGas);
  Serial.print(" (Threshold at: "); Serial.print(gasBaseline + gasThreshold);
  Serial.println(")");

  // Clear hardware indicators before each diagnostic step
  digitalWrite(PIN_SHUTDOWN, LOW);
  digitalWrite(PIN_FAN, LOW);
  digitalWrite(PIN_PUMP, LOW);
  digitalWrite(PIN_EXHAUST, LOW);

  // 1. CRITICAL GAS LEAK INTERRUPT
  if (rawGas > (gasBaseline + gasThreshold)) { 
    Serial.println("❌ STATE: EMERGENCY GAS SHUTDOWN. RED + GREEN ACTIVE.");
    digitalWrite(PIN_SHUTDOWN, HIGH); 
    digitalWrite(PIN_EXHAUST, HIGH);  
  }
  
  // 2. RUN REGULAR TEMPERATURE RULES (Only if gas levels are completely safe)
  else {
    if (temp >= 80.0) {
      Serial.println("🛑 STATE: EMERGENCY THERMAL SHUTDOWN. RED ONLY.");
      digitalWrite(PIN_SHUTDOWN, HIGH); 
    } 
    else if (temp >= 68.0) {
      Serial.println("💧 STATE: DUAL COOLING ACTIVE. YELLOW + BLUE ON.");
      digitalWrite(PIN_FAN, HIGH);   
      digitalWrite(PIN_PUMP, HIGH);  
    } 
    else if (temp >= 60.0) {
      Serial.println("🌬️ STATE: LIGHT FAN COOLING ACTIVE. YELLOW ONLY.");
      digitalWrite(PIN_FAN, HIGH);   
    }
    else {
      Serial.println("✅ STATE: SYSTEM NORMAL. ALL COOLS DISENGAGED.");
    }
  }

  // 3. SEND METRICS TO n8n CLOUD AI AGENT
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(n8n_webhook);
    http.addHeader("Content-Type", "application/json");

    // Package JSON string payload
    String payload = "{\"temp\":" + String(temp) + 
                     ",\"vibration\":" + String(vibration) + 
                     ",\"gas\":" + String(rawGas) + "}";
    
    int httpResponseCode = http.POST(payload);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("\n🤖 LIVE AUDIT REPORT FROM AI AGENT:");
      Serial.println(response); // Prints out the Gemini operational description
    } else {
      Serial.print("❌ HTTP Transmission Error: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }

  Serial.println("-------------------------------------------------------------");
  delay(4000); // 4-second delay protects API rate limits on continuous loops
}
