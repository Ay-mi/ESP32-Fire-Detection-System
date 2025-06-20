#include <DHT.h>

#define DHTPIN 4           //GPIO pin
#define DHTTYPE DHT22      //Either DHT11
#define CO2_PIN 34         // MQ sensor
#define LDR_PIN 35         //Light sensor
#define PUMP_PIN 26        //Water pump
#define BUZZER_PIN 25      //buzzer

DHT dht(DHTPIN, DHTTYPE);

void readSensors() {
  // Read temperature and humidity
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (!isnan(temp) && !isnan(humidity)) {
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print(" °C | Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  } else {
    Serial.println("Failed to read from DHT sensor!");
  }

  // Read CO2 sensor
  int co2Value = analogRead(CO2_PIN);  
  Serial.print("CO2 Level: ");
  Serial.println(co2Value);

  // Read light sensor
  int ldrValue = analogRead(LDR_PIN);  
  Serial.print("Light Intensity: ");
  Serial.println(ldrValue);

  // Check for sudden light spike
  if (ldrValue > 3000) {
    Serial.println("ALERT: Sudden light spike detected! Possible fire.");
  }
}

void updateFireStatus() {
  int co2Value = analogRead(CO2_PIN);
  int ldrValue = analogRead(LDR_PIN);
  float temp = dht.readTemperature();

  int fireLevel = 0;  // 0 = Normal, 1 = Warning, 2 = Fire

  if (co2Value > 3000 || ldrValue > 3000 || temp > 120) {
    fireLevel = 2;  // FIRE
    Serial.println("ALERT: FIRE DETECTED!");
  } else if (co2Value > 2000 || ldrValue > 2000 || temp > 90) {
    fireLevel = 1;  // WARNING
    Serial.println("WARNING: Possible fire conditions detected");
  }

  Serial.print("Current Fire Status Level: ");
  Serial.println(fireLevel);

  controlPump(fireLevel);
  controlBuzzer(fireLevel);
}

void controlPump(int fireLevel) {
  if (fireLevel == 2) {
    digitalWrite(PUMP_PIN, HIGH);   // Activate pump
    Serial.println("Water pump ACTIVATED");
  } else {
    digitalWrite(PUMP_PIN, LOW);    // Deactivate
    Serial.println("Water pump deactivated");
  }
}

void controlBuzzer(int fireLevel) {
  if (fireLevel == 2) {
    digitalWrite(BUZZER_PIN, HIGH);   // Activate buzzer
    Serial.println("Buzzer ACTIVATED");
  } else {
    digitalWrite(BUZZER_PIN, LOW);    // Deactivate
    Serial.println("Buzzer deactivated");
  }
}

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
  
  Serial.println("System ready. Starting sensor monitoring...");
}

void loop() {
  readSensors();
  updateFireStatus();
  delay(2000);  // Wait 2 seconds between readings
}