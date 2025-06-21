#define BLYNK_TEMPLATE_ID "TMPL6e29v4i47"
#define BLYNK_TEMPLATE_NAME "Smart Kitchen Fire Detector"
#define BLYNK_AUTH_TOKEN "0ghgktnIzQHRCeH700vJ-RhIZcpEUkai"

#include <WiFi.h>                // Includes the Wi-Fi library for the ESP32
#include <WiFiClient.h>          // Includes the Wi-Fi client library for network connections
#include <BlynkSimpleEsp32.h>    // Includes the Blynk library specific to ESP32
#include <string.h>              // Includes the library for using string functions
#include <DHT.h>

#define BLYNK_PRINT Serial       // Enable debug printing to serial monitor

#define DHTPIN 4           //GPIO pin
#define DHTTYPE DHT11      //Either DHT11
#define CO2_PIN 34         // MQ sensor
#define LDR_PIN 35         //Light sensor
#define PUMP_PIN 26        //Water pump //this is not going to work like this
#define BUZZER_PIN 25      //buzzer

DHT dht(DHTPIN, DHTTYPE);
#define INTERVAL 3000            // Interval at which to send sensor data (milliseconds)
unsigned long previousMillis = 0; // Stores last time sensor data was sent

char auth[] = BLYNK_AUTH_TOKEN;  // Blynk authorization token
char ssid[] = "Redmi_Note_14";                // WiFi SSID
char pass[] = "a6gqjkm67c78mdk";                // WiFi password

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing Smart Kitchen Fire Detector...");
  
  dht.begin();
  
  pinMode(CO2_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);  
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);
  
  // Ensure all actuators are off initially
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(PUMP_PIN, LOW);
  Blynk.begin(auth, ssid, pass); // Connect to Blynk
  Serial.println("System ready. Starting sensor monitoring...");
}
void loop() {
  int fireAlert = 0;
  Blynk.run();                   // Run Blynk process
  unsigned long currentMillis = millis(); // Get the current time
  // read sensors in loop and send information to Blynk
  //DHT11
  // Check if it is time to read and send sensor data
  if (currentMillis - previousMillis >= INTERVAL) {
    previousMillis = currentMillis;       // Update the last sent time
    float t = dht.readTemperature();      // Read temperature from DHT sensor
    float h = dht.readHumidity();         // Read humidity from DHT sensor

    // Check if DHT readings were successful
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!\n");
    } else {
      Blynk.virtualWrite(V0, t);          // Send temperature to Blynk
      Blynk.virtualWrite(V1, h);          // Send humidity to Blynk
      BLYNK_PRINT.printf("Humidity: %.2f, Temperature: %.2f ", h, t);
      
    }

  //LDR
  int ldrValue = analogRead(LDR_PIN);
  Serial.print("\n Light Intensity: ");
  Serial.println(ldrValue);
  Blynk.virtualWrite(V2, ldrValue);          // Send LDR reading to Blynk

  //MQ2
  int co2Value = analogRead(CO2_PIN);
  Blynk.virtualWrite(V3, co2Value);          // Send CO2 reading to Blynk
  Serial.print("CO2 Level: ");
  Serial.println(co2Value);

  //make decision
  fireAlert = decision(co2Value, ldrValue, t, h);
  Serial.print("Fire Alert: ");
  Serial.println(fireAlert);
}}

//decision function
//since dht11 is not reading, we'll only use mq2 and ldr values for now
int decision(int co2, int ldr, float t, float hum){
  int co2Value = co2;
  int ldrValue = ldr;
  float temp = t;
  float humidity = hum;
  int fireAlert = 0;
  if(co2Value > 3000 || ldrValue > 3000 || temp > 120 || humidity < 20){
    fireAlert = 2;
  }
  else if(co2Value > 2500 || ldrValue > 2500 || temp > 100 || humidity < 40){
    fireAlert = 1;
  }
  return fireAlert;
}

