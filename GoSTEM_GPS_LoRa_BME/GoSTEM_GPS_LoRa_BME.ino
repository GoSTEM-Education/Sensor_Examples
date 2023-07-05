/*
  Read NMEA sentences over sERIAL using Ublox module SAM-M8Q, NEO-M8P, etc
  Base on SparkFun_Ublox_Arduino_Library //https://github.com/sparkfun/SparkFun_Ublox_Arduino_Library
*/

//Codi GPS-LoRa

#include "SparkFun_Ublox_Arduino_Library.h"
#include <LoRa.h>
#include "board_LoRa.h"

SFE_UBLOX_GPS myGPS;

#include <MicroNMEA.h> //https://github.com/stevemarple/MicroNMEA

char nmeaBuffer[100];
MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));

//Codi BME 280

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>



#define SEALEVELPRESSURE_HPA (1013.25)

//BME280 definition
#define SDA 21
#define SCL 22
TwoWire I2Cone = TwoWire(1);

Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

unsigned long delayTime;



void setup()
{

   Serial.begin(115200);
    initBoard();
    // When the power is turned on, a delay is required.
    delay(1500);

    Serial.println("GOSTEM GPS + LoRa Example");

    if (myGPS.begin(Serial1) == false) {
        Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
        while (1);
    }
    LoRa.setPins(RADIO_CS_PIN, RADIO_RST_PIN, RADIO_DI0_PIN);
    

    if (!LoRa.begin(LoRa_frequency)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }
    LoRa.setSpreadingFactor(10);           // ranges from 6-12,default 7 see API docs

    while(!Serial);    // time to get serial running
    Serial.println(F("BME280 test"));

    unsigned status;
    
    // default settings
    //status = bme.begin();
    I2Cone.begin(SDA, SCL, 100000);  
    // You can also pass in a Wire library object like &Wire2
     status = bme.begin(0x76, &I2Cone);
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("        ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
    }
    
    Serial.println("-- Default Test --");
    delayTime = 1000;

    Serial.println();



}

void loop()
{


    myGPS.checkUblox(); //See if new data is available. Process bytes as they come in.

    if (nmea.isValid() == true) {
        long latitude_mdeg = nmea.getLatitude();
        long longitude_mdeg = nmea.getLongitude();
        long speed = nmea.getSpeed();
        long alt;


         // Define LoRa packet - Start
        LoRa.beginPacket();
        LoRa.println("----- Comms GPS------");
        LoRa.print("Latitude (deg): ");
        LoRa.println(latitude_mdeg / 1000000., 6);
        LoRa.print("Longitude (deg): ");
        LoRa.println(longitude_mdeg / 1000000., 6);
        LoRa.print("Speed (m/s): ");

        LoRa.println(speed * 0.0005144444444);
        if (nmea.getAltitude(alt))
          LoRa.print("Altitude m: ");
          LoRa.println(alt / 1000., 3);


        LoRa.print("Date/time: ");
        LoRa.print(int(nmea.getDay()));
        LoRa.print('-');
        LoRa.print(int(nmea.getMonth()));
        LoRa.print('-');
        LoRa.print(nmea.getYear());
        LoRa.print("       T: ");
        LoRa.print(int(nmea.getHour() + 2));
        LoRa.print(':');
        LoRa.print(int(nmea.getMinute()));
        LoRa.print(':');
        LoRa.println(int(nmea.getSecond()));
        LoRa.println();
        LoRa.endPacket();



        Serial.println("------ Packet LoRa GPS Enviat ------");
        Serial.print("Latitude (deg): ");
        Serial.println(latitude_mdeg / 1000000., 6);
        Serial.print("Longitude (deg): ");
        Serial.println(longitude_mdeg / 1000000., 6);
        Serial.print("Speed (m/s): ");
        Serial.println(speed * 0.0005144444444);
        if (nmea.getAltitude(alt))
          Serial.print("Altitude m: ");
          Serial.println(alt / 1000., 3);


        Serial.print("Date/time: ");
        Serial.print(int(nmea.getDay()));
        Serial.print('-');
        Serial.print(int(nmea.getMonth()));
        Serial.print('-');
        Serial.print(nmea.getYear());
        Serial.print("       T: ");
        Serial.print(int(nmea.getHour() + 2));
        Serial.print(':');
        Serial.print(int(nmea.getMinute()));
        Serial.print(':');
        Serial.println(int(nmea.getSecond()));  
        Serial.println();

        delay(1500);
    } else {
        LoRa.beginPacket();
        LoRa.print("Sense senyal GPS");
        LoRa.println(nmea.getNumSatellites());
        LoRa.endPacket();

        Serial.print("No Fix - Sat ");
        Serial.println(nmea.getNumSatellites());
        delay(1000);
    }

    delay(250); //Don't pound too hard on the I2C bus


        LoRa.beginPacket();
        LoRa.println("------Comms BME------");
    LoRa.print("Temperature = ");
    LoRa.print(bme.readTemperature());
    LoRa.println(" °C");
    LoRa.print("Pressure = ");
    LoRa.print(bme.readPressure() / 100.0F);
    LoRa.println(" hPa");
    LoRa.print("Approx. Altitude = ");
    LoRa.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    LoRa.println(" m");
    LoRa.print("Humidity = ");
    LoRa.print(bme.readHumidity());
    LoRa.println(" %");
    LoRa.println();
    LoRa.endPacket();


    Serial.println("------ Packet LoRa BME Enviat ------");
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" °C");

    Serial.print("Pressure = ");

    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();
}

//This function gets called from the SparkFun Ublox Arduino Library
//As each NMEA character comes in you can specify what to do with it
//Useful for passing to other libraries like tinyGPS, MicroNMEA, or even
//a buffer, radio, etc.
void SFE_UBLOX_GPS::processNMEA(char incoming)
{
    //Take the incoming char from the Ublox I2C port and pass it on to the MicroNMEA lib
    //for sentence cracking
    nmea.process(incoming);
}
