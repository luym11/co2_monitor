/**
 * CO2 Monitor Test
 *
 * Simple test to verify the Sensirion SCD4x library compiles correctly.
 */
#include "Arduino.h"
#include "SensirionI2cScd4x.h"

// Set LED_BUILTIN if it is not defined by Arduino framework
#define LED_BUILTIN 13

// Create sensor instance
SensirionI2cScd4x scd4x;

void setup()
{
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("CO2 Monitor Test Starting...");
  
  // Initialize LED digital pin as an output
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Initialize I2C
  Wire.begin();
  
  // Initialize the SCD4x sensor
  scd4x.begin(Wire, SCD40_I2C_ADDR_62);
  
  Serial.println("SCD4x sensor initialized");
}

void loop()
{
  // Blink LED to show the program is running
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  
  Serial.println("Library test - LED blinking");
}