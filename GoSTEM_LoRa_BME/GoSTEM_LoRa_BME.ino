/***************************************************************************
GoSTEM 2023
 ***************************************************************************/
#include <LoRa.h>
#include "boards.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


//Define Pins for Sensor
#define SDA 21
#define SCL 22
TwoWire I2Cone = TwoWire(1);

Adafruit_BME280 bme; // I2C
#define SEALEVELPRESSURE_HPA (1013.25)

int counter = 0;

void setup()
{
    Serial.begin(115200);
    initBoard();
    // When the power is turned on, a delay is required.
    delay(1500);

    Serial.println("LoRa Sender");
    LoRa.setPins(RADIO_CS_PIN, RADIO_RST_PIN, RADIO_DIO0_PIN);
    if (!LoRa.begin(LoRa_frequency)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }
    LoRa.setSpreadingFactor(7);      // ranges from 6-12, default 7 see API doc

      unsigned status;
      I2Cone.begin(SDA, SCL,100000);  
      status = bme.begin(0x76, &I2Cone);
  
  if (!status) {
    Serial.println(F("Could not find a valid BME280 sensor, check wiring or try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("        ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }


}

void loop()
{
    Serial.print("Sending packet: ");
    Serial.println(counter);

    // send packet
    SendValues();

    printValues();

#ifdef HAS_DISPLAY
    if (u8g2) {
        char buf[256];
        u8g2->clearBuffer();
        u8g2->drawStr(0, 12, "Transmitting: OK!");
        snprintf(buf, sizeof(buf), "Sending: %d", counter);
        u8g2->drawStr(0, 30, buf);
        u8g2->sendBuffer();
    }
#endif
    counter++;
    delay(5000);
}

void SendValues() {
    LoRa.beginPacket();
    String messageTemp = "Temp: "+String(bme.readTemperature())+" C";
    LoRa.print(messageTemp);
    LoRa.endPacket();
 
}

void printValues() {
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" °C");

    Serial.print("Pressure = ");

    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.println();
}