#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_MAX31855.h>

#define MAXDO   24
#define MAXCS   52
#define MAXCLK  22

// I2C LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Adjust the I2C address if needed

// Initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);  // Initialize the LCD
  lcd.backlight();

  while (!Serial) delay(1); // wait for Serial on Leonardo/Zero, etc

  Serial.println("MAX31855 test");
  // wait for MAX chip to stabilize
  delay(500);
  Serial.print("Initializing sensor...");
  if (!thermocouple.begin()) {
    Serial.println("ERROR.");
    while (1) delay(10);
  }

  Serial.println("DONE.");
}

void loop() {
  // Basic readout test, print the current temp to Serial and LCD
  Serial.print("Internal Temp = ");
  Serial.println(thermocouple.readInternal());

  double f = thermocouple.readFahrenheit();
  if (isnan(f)) {
    Serial.println("Thermocouple fault(s) detected!");
    uint8_t e = thermocouple.readError();
    if (e & MAX31855_FAULT_OPEN) Serial.println("FAULT: Thermocouple is open - no connections.");
    if (e & MAX31855_FAULT_SHORT_GND) Serial.println("FAULT: Thermocouple is short-circuited to GND.");
    if (e & MAX31855_FAULT_SHORT_VCC) Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
  } else {
    // Print temperature on both Serial and LCD
    Serial.print("F = ");
    Serial.println(f);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(f);
    lcd.print("F");
  }

  delay(3000);  // Display update every 3 seconds
}
