#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_MAX31855.h>

#define MAXDO   24
#define MAXCS   52
#define MAXCLK  22
#define Pin_LED_Red 40
#define Pin_LED_Green 36

// initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address 0x27, 16 columns, 2 rows

void setup() {
  lcd.init();                      // initialize the lcd 
  lcd.init();
  Serial.begin(9600);

  while (!Serial); // wait for Serial on Leonardo/Zero, etc

  pinMode(Pin_LED_Red, OUTPUT);
  pinMode(Pin_LED_Green, OUTPUT);

  lcd.begin(16, 2);  // Initialize the LCD
  lcd.backlight();

  Serial.println("MAX31855 test");
  // wait for MAX chip to stabilize
  delay(500);
  Serial.print("Initializing sensor...");

  if (!thermocouple.begin()) {
    Serial.println("ERROR: Thermocouple not initialized.");
    while (1) delay(10);
  }

  Serial.println("DONE.");
}

void loop() {
  // Your existing loop code here

  // basic readout test, just print the current temp
  Serial.print("Internal Temp = ");
  Serial.println(thermocouple.readInternal());

  double c = thermocouple.readCelsius();
   if (isnan(c)) {
    Serial.println("Error reading thermocouple!");
    Serial.print("Error Code: ");
    Serial.println(thermocouple.readError());
    lcd.setCursor(0, 0);
    lcd.print("Error reading TC!");
    lcd.setCursor(0, 1);
    lcd.print("Error Code: ");
    lcd.print(thermocouple.readError());
  } else {
    Serial.print("C = ");
    Serial.println(c);
    Serial.print("F = ");
    double f = thermocouple.readFahrenheit();
    Serial.println(f);
     // Print temperature on LCD
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(f);
    lcd.print("F");

    lcd.setCursor(0, 1);
    lcd.print("Int Temp: ");
    lcd.print(c);
    lcd.print("C");
  }

  // Add your LED control code here
  if (thermocouple.readFahrenheit() > 80.0) {
    // Turn on the green LED
    digitalWrite(Pin_LED_Green, HIGH);
    // Turn off the red LED
    digitalWrite(Pin_LED_Red, LOW);
  } else {
    // Turn on the red LED
    digitalWrite(Pin_LED_Red, HIGH);
    // Turn off the green LED
    digitalWrite(Pin_LED_Green, LOW);
  }

  delay(1000);
}
