#include "Arduino.h"
#include "SensirionI2cScd4x.h"
#include "Wire.h"
#include <U8g2lib.h>
#include <string.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

// Define LED pins
#define GREEN_LED 12
#define BLUE_LED 11
#define YELLOW_LED 10

// OLED Display - GME12864-70 with SH1106 controller
// Using U8g2 library for SH1106 I2C (address 0x3C or 0x3D)
U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

SensirionI2cScd4x scd4x;

void setup()
{
  Serial.begin(115200);
  while (!Serial) {
    delay(100);
  }
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  
  Wire.begin();
  
  // Initialize OLED display (GME12864-70 with SH1106)
  display.begin();
  display.clearBuffer();
  display.sendBuffer();
  
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
  if (co2 < 800) {
    // CO2 below 800: Green LED on, others off
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(BLUE_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
  } else if (co2 >= 800 && co2 <= 1000) {
    // CO2 between 800-1000: Blue LED on, others off
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(BLUE_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
  } else {
    // CO2 above 1000: Yellow LED on, others off
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(BLUE_LED, LOW);
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
  
  // Update OLED display
  display.clearBuffer();
  
  // Display CO2 value in large text (centered)
  display.setFont(u8g2_font_logisoso42_tn);
  char co2Str[6];
  sprintf(co2Str, "%d", co2);
  // Center the text (approximately)
  int textWidth = strlen(co2Str) * 24; // Approximate width per digit
  int xPos = (128 - textWidth) / 2;
  display.setCursor(xPos, 50);
  display.print(co2Str);
  
  display.sendBuffer();
}