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
#define PUMP_RELAY_PIN 26        //Water pump //this is not going to work like this
#define BUZZER_PIN 25      //buzzer

#define LED1 16
#define LED2 17
#define LED3 18

DHT dht(DHTPIN, DHTTYPE);
#define INTERVAL 3000            // Interval at which to send sensor data (milliseconds)
unsigned long previousMillis = 0; // Stores last time sensor data was sent

char auth[] = BLYNK_AUTH_TOKEN;  // Blynk authorization token
char ssid[] = "Redmi_Note_14";                // WiFi SSID
char pass[] = "xxx";                // WiFi password

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing Smart Kitchen Fire Detector...");
  
  dht.begin();
  
  pinMode(CO2_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);  
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  Blynk.begin(auth, ssid, pass); // Connect to Blynk
  Serial.println("System ready. Starting sensor monitoring...");

  //testing system
  digitalWrite(LED1, HIGH);
  delay(500);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, HIGH);
  delay(500);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, HIGH);
  delay(500);
  digitalWrite(LED3, LOW);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW); 
  delay(300);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);
  //end of test

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(PUMP_RELAY_PIN, LOW);

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
  Serial.print("\nLight Intensity: ");
  Serial.println(ldrValue);
  Blynk.virtualWrite(V2, ldrValue);          // Send LDR reading to Blynk

  //MQ2
  int co2Value = analogRead(CO2_PIN);
  Blynk.virtualWrite(V3, co2Value);          // Send CO2 reading to Blynk
  Serial.print("CO2 Level: ");
  Serial.println(co2Value);

  //make decision
  decision(co2Value, ldrValue, t, h);
  Serial.print("Fire Alert: ");
  Serial.println(fireAlert);
  delay(10000);
}}

//decision function
void decision(int co2, int ldr, float t, float hum){
  int co2Value = co2;
  int ldrValue = ldr;
  float temp = t;
  float humidity = hum;
  int fireAlert = 0;
  if(co2Value > 3000 || ldrValue > 4000 || temp > 120 || humidity < 20){
    fireAlert = 2;
  }
  else if(co2Value > 2500 || ldrValue > 3500 || temp > 70 || humidity < 40){
    fireAlert = 1;
  }

  //LED Control
  if(temp > 70 || humidity < 40){
    digitalWrite(LED1, HIGH);
  }
  if(co2Value > 2500){
    digitalWrite(LED2, HIGH);
  }
  if(ldrValue > 3500){
    digitalWrite(LED3, HIGH);
  }

  //Water pump and buzzer control
  if(fireAlert == 2){
    Serial.print("LEDs ON\n");
    digitalWrite(BUZZER_PIN, HIGH);   // Activate buzzer
    Serial.println("Buzzer activated");
    delay(5000);
    digitalWrite(BUZZER_PIN, LOW);    // Deactivate
    Serial.println("Buzzer deactivated");
    digitalWrite(PUMP_RELAY_PIN, HIGH);
    Serial.println("Water pump activated");
    delay(10000);
    digitalWrite(PUMP_RELAY_PIN, LOW);
    Serial.println("Water pump deactivated");
    delay(5000);

  }
  else if(fireAlert == 1){
    Serial.print("LEDs ON\n");
    digitalWrite(BUZZER_PIN, LOW);    // Deactivate
    Serial.println("Buzzer deactivated");
    digitalWrite(PUMP_RELAY_PIN, LOW);
    Serial.println("Water pump deactivated");
    delay(5000);
  }
  else{
    Serial.print("LEDs OFF\n");
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(BUZZER_PIN, LOW);    // Deactivate
    Serial.println("Buzzer deactivated");
    digitalWrite(PUMP_RELAY_PIN, LOW);
    Serial.println("Water pump deactivated");
    delay(5000);
  }
}
