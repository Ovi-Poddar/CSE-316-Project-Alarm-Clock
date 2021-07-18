// Read and write data to and from an SD card using Arduino
 
#include <SPI.h>        // Include SPI library (needed for the SD card)
#include <SD.h>         // Include SD library

File myFile;
char str[10];
 
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  } 
  if (!SD.begin()) {
    Serial.write("Initialization failed!");
    while (1);
  }
}
void loop()
{
    myFile = SD.open("test100.txt");
    if (myFile) {
      int i=0;
      while (myFile.available()) {
        Serial.write(myFile.read());
        //char c = myFile.read();
        //str[i++] = c;
      }
    }
    myFile.close();
    delay(50);
}
