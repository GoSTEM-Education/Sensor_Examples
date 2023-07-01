/*
  Read NMEA sentences over sERIAL using Ublox module SAM-M8Q, NEO-M8P, etc
  Base on SparkFun_Ublox_Arduino_Library //https://github.com/sparkfun/SparkFun_Ublox_Arduino_Library
*/

#include "SparkFun_Ublox_Arduino_Library.h"
#include <LoRa.h>
#include "board_LoRa.h"

SFE_UBLOX_GPS myGPS;

#include <MicroNMEA.h> //https://github.com/stevemarple/MicroNMEA

char nmeaBuffer[100];
MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));

void setup()
{
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
    LoRa.setSpreadingFactor(7);           // ranges from 6-12,default 7 see API docs
}

void loop()
{
    myGPS.checkUblox(); //See if new data is available. Process bytes as they come in.

    if (nmea.isValid() == true) {
        long latitude_mdeg = nmea.getLatitude();
        long longitude_mdeg = nmea.getLongitude();
        long altitude = nmea.getAltitude(altitude);
        long speed = nmea.getSpeed();
        long course =  nmea.getCourse();



         // Define LoRa packet - Start
        LoRa.beginPacket();
        LoRa.print("La: ");
        LoRa.println(latitude_mdeg / 1000000., 6);
        LoRa.print("Lo: ");
        LoRa.println(longitude_mdeg / 1000000., 6);
        LoRa.print("Alt: "); //Altitude en metres
        LoRa.println(altitude / 1000., 3);
        LoRa.print("V: ");
        LoRa.println(speed / 1000., 3);
        LoRa.print("D: ");
        LoRa.println(course / 1000., 3);
        LoRa.endPacket();
        // Define LoRa packet - End
        Serial.println("------ Packet LoRa Enviat ------");

        Serial.print("Lat: ");
        Serial.println(latitude_mdeg / 1000000., 6);
        Serial.print("Long: ");
        Serial.println(longitude_mdeg / 1000000., 6);

        Serial.print("Alt: "); //Altitude en metres
        Serial.println(altitude / 1000., 3);

        Serial.print("Speed: ");
        Serial.println(speed / 1000., 3);
        Serial.print("Course: ");
        Serial.println(course / 1000., 3);

        delay(5000);
    } else {
        LoRa.beginPacket();
        LoRa.print("N_Fx-S ");
        LoRa.println(nmea.getNumSatellites());
        LoRa.endPacket();

        Serial.print("No Fix - Sat ");
        Serial.println(nmea.getNumSatellites());
        delay(1000);
    }

    delay(250); //Don't pound too hard on the I2C bus
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
