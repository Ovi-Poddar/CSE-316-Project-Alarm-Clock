// Read and write data to and from an SD card using Arduino
 
#include <SPI.h>        // Include SPI library (needed for the SD card)
#include <SD.h>         // Include SD library

File myFile;
 
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
 
  Serial.print("Initializing SD card...");
 
  if (!SD.begin()) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
 
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test100.txt",  FILE_WRITE);
 
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to FILE...");
    myFile.println("Writing a message.");
    // close the file:
    myFile.close();
    Serial.println("Writing done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("Error opening the file.");
  }
 
  // re-open the file for reading:
  myFile = SD.open("test100.txt");
  if (myFile) {
    Serial.println("Reading from file : ");
 
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("Error opening the file.");
  }
}
 
void loop() {
  
}
