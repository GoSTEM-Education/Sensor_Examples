//EXEMPLER PER A LORA MINI ESP32 AMB UN SD CARD READER SIMPLE
#include <SPI.h>
#include <SD.h>

#define SD_CS_PIN 5    // CS pin connected to GPIO 5 (D1)
#define SD_SCK_PIN 18  // SCK pin connected to GPIO 18
#define SD_MOSI_PIN 23 // MOSI pin connected to GPIO 23
#define SD_MISO_PIN 19 // MISO pin connected to GPIO 19

SPIClass * hspi = NULL;

File GoSTEM_LOG;
int current_size = 0;


void setup() {
  Serial.begin(115200);
  // Initialize the SPI interface
  hspi = new SPIClass(HSPI);
  hspi->begin(SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
  pinMode(SD_CS_PIN, OUTPUT);

  // Initialize the SD card
 if (!SD.begin(SD_CS_PIN,*hspi)) {
    Serial.println("SD card initialization failed!");
    while (1);
  }

  Serial.println("SD card initialized.");
}

void loop() {
   
    if (SD.exists("/data.txt")) { // El nom per defecte del nostre fixer de dades es data.csv
      Serial.println("data.txt exists.");
      // Open the file in write mode
      GoSTEM_LOG = SD.open("/data.txt", FILE_WRITE);
      // If the file opened successfully, write the data
      if (GoSTEM_LOG) {
        GoSTEM_LOG.seek(current_size);
        GoSTEM_LOG.println("1,2,3,4,5");  // Write the dummy data
        current_size = current_size+10;
        GoSTEM_LOG.close();
        Serial.println("Data saved!");
      }
      else {
        Serial.println("Error writing data.txt!");}
    }
    else {
      Serial.println("data.txt doesn't exist.");
        // open a new file and immediately close it:
          Serial.println("Creating data.txt...");
          GoSTEM_LOG = SD.open("/data.txt", FILE_WRITE);
          GoSTEM_LOG.println("--- LOG Begin ---");
          GoSTEM_LOG.close();
    }

delay(5000);

}
