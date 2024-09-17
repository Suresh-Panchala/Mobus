#include <ModbusMaster.h>
#include <SoftwareSerial.h>

// Instantiate ModbusMaster object
ModbusMaster node;

// Pin definitions for RS485 module
#define MAX485_DE 3
#define MAX485_RE 2

// Define RX and TX pins for SoftwareSerial
#define RX_PIN 10  // Receive pin for RS485
#define TX_PIN 11  // Transmit pin for RS485

// Create a SoftwareSerial instance
SoftwareSerial RS485Serial(RX_PIN, TX_PIN);  // RX, TX

// Enable/Disable RS485 transmission
void preTransmission()
{
  digitalWrite(MAX485_RE, 1);
  digitalWrite(MAX485_DE, 1);
}

void postTransmission()
{
  digitalWrite(MAX485_RE, 0);
  digitalWrite(MAX485_DE, 0);
}

void setup() {
  // Initialize pins for RS485
  pinMode(MAX485_RE, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  
  // Set RE and DE low to disable RS485 transmission initially
  digitalWrite(MAX485_RE, 0);
  digitalWrite(MAX485_DE, 0);

  // Start the hardware Serial (for debugging)
  Serial.begin(9600);

  // Start SoftwareSerial for RS485 communication
  RS485Serial.begin(9600);  // Set the baud rate for Modbus RTU

  // Set the Modbus slave ID of the target device
  node.begin(4, RS485Serial);  // 1 is the slave ID, change as needed

  // Attach pre/post transmission functions for RS485 communication
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}

void loop() {
  uint8_t result;
  uint16_t data[10]; // Array to hold the read data
  
  // Read 10 input registers starting at address 100 (0x0064)
  result = node.readInputRegisters(0x0000, 10);

  // Process the result
  if (result == node.ku8MBSuccess) {
    for (int i = 1; i < 8; i++) {
      data[i] = node.getResponseBuffer(i);
      uint32_t combined = ((uint32_t)data[i] << 16) | data[0];
      float value;
      memcpy(&value, &combined, sizeof(value));
      Serial.print("Floating-point value: ");
      Serial.println(value);
      i++;
    }
  } else {
    Serial.print("Error reading Modbus: ");
    Serial.println(result, HEX);
  }
  Serial.println("//////////////////////////////");
  delay(1000); // Wait for a second before next read
}
