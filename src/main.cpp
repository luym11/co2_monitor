#include "Arduino.h"
#include "SensirionI2cScd4x.h"
#include "Wire.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

// Define LED pins
#define GREEN_LED 12
#define YELLOW_LED 11

SensirionI2cScd4x scd4x;

void setup()
{
  Serial.begin(115200);
  while (!Serial) {
    delay(100);
  }
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  
  Wire.begin();
  
  scd4x.begin(Wire, SCD41_I2C_ADDR_62);
  
  int16_t error = scd4x.wakeUp();
  if (error != 0) {
    return;
  }
  
  error = scd4x.stopPeriodicMeasurement();
  if (error != 0) {
    // ignore error
  }
  
  error = scd4x.startPeriodicMeasurement();
  if (error != 0) {
    return;
  }
  
  delay(5000);
}

void loop()
{
  bool dataReady = false;
  uint16_t co2 = 0;
  float temp = 0.0;
  float humidity = 0.0;
  
  delay(5000);
  
  int16_t error = scd4x.getDataReadyStatus(dataReady);
  if (error != 0) {
    return;
  }
  
  while (!dataReady) {
    delay(100);
    error = scd4x.getDataReadyStatus(dataReady);
    if (error != 0) {
      return;
    }
  }
  
  error = scd4x.readMeasurement(co2, temp, humidity);
  if (error != 0) {
    return;
  }
  
  // Control LEDs based on CO2 level
  if (co2 < 700) {
    // CO2 below 700: Green LED on, Yellow LED off
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
  } else if (co2 >= 700 && co2 <= 1000) {
    // CO2 between 700-1000: Yellow LED on, Green LED off
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
  } else {
    // CO2 above 1000: Both LEDs on
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, HIGH);
  }
  
  // Blink built-in LED to show measurement complete
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  
  Serial.print("Time: ");
  Serial.print(millis() / 1000);
  Serial.print("s | CO2: ");
  Serial.print(co2);
  Serial.print(" ppm | Temp: ");
  Serial.print(temp, 1);
  Serial.print("C | Humidity: ");
  Serial.print(humidity, 1);
  Serial.println("%");
}