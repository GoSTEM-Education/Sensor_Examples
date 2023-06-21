// Code Example for Moisture capacitive sensor in LilyGo LoRa ESP32

int sensorPin = 34; // Analog input pin connected to the moisture sensor

void setup() {
  Serial.begin(115200); // Initialize serial communication at 9600 baud rate
}

void loop() {
  int moistureLevel = analogRead(sensorPin); // Read moisture level from sensor
  int moisturePercent = map(moistureLevel, 4095, 1350, 0, 100); // Map moisture level to a percentage
  
  Serial.print("Moisture level: ");
  Serial.print(moisturePercent);
  Serial.println("%");
  
  delay(1000); // Wait for 1 second
}
  