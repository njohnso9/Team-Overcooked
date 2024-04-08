#include <RBDdimmer.h>
#include <Wire.h>
#include <Adafruit_MAX31855.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define MAX_THERMOCOUPLES 9
#define MAX_DIGITS 3

// Define pin assignments for shared T0, T1, T2, SO, CS, SCK pins
const int T0_PIN = 33;
const int T1_PIN = 31;
const int T2_PIN = 29;
const int SO_PIN = 27;
const int CS_PIN = 25;
const int SCK_PIN = 23;
const int SCK_PIN2 = 22;
const int SO_PIN2 = 26;
const int CS_PIN2 = 24;

// Adafruit_MAX31855 instance for the shared chip
Adafruit_MAX31855 thermocouple(SCK_PIN, CS_PIN, SO_PIN);
Adafruit_MAX31855 thermocouple2(SCK_PIN2, CS_PIN2, SO_PIN2);

//Summer assigning therm
float a, b, c, d, e, f, g, h, i;
float weightedAverage = 0;
int summer = 1;

// Dock Initialize a variable to store the input temperature
String inputTemperatureStr = "";

//Digit count for keypad
int digitCount = 0;

// Initialize a variable to store the current temperature
float currentTemperature = weightedAverage;

// Define the number of rows and columns for the keypad
const byte ROWS = 4;
const byte COLS = 4;

// Define the keypad layout
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Define the row and column pins for the keypad
byte rowPins[ROWS] = {8, 9, 10, 11};
byte colPins[COLS] = {4, 5, 6, 7};

// Define pins
const int PAC_TIME = 2; //J-man
const int PAC_OUT = 12; // J-man
const int CLEAR_INT = 3; // clear interrupt button

// Define the pins for each individual LED  
const int LED1 = 40; //starting from bot right
const int LED2 = 50;
const int LED3 = 48;
const int LED4 = 42;
const int LED5 = 51;
const int LED6 = 49;
const int LED7 = 47;
const int LED8 = 45;
const int LED9 = 43; //end top left

// Initialize keypad and LCD
Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x27, 20, 4);

dimmerLamp dimmer(PAC_OUT); // initialize dimmer object

void setup() {
  Serial.begin(9600);
  dimmer.begin(NORMAL_MODE, ON); // setup PAC module
  // Initialize shared T0, T1, T2, CS, SCK pins
  pinMode(T0_PIN, OUTPUT);
  pinMode(T1_PIN, OUTPUT);
  pinMode(T2_PIN, OUTPUT);
  pinMode(CS_PIN, OUTPUT);
  pinMode(SCK_PIN, OUTPUT);
  pinMode(CS_PIN2, OUTPUT);
  pinMode(SCK_PIN2, OUTPUT);
 
  // Set the mode of each LED pin as OUTPUT
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);
  pinMode(LED7, OUTPUT);
  pinMode(LED8, OUTPUT);
  pinMode(LED9, OUTPUT);
 
  //set mode for PAC & Interrupt button
  pinMode(PAC_TIME, OUTPUT);
  pinMode(PAC_OUT, OUTPUT);
  pinMode(CLEAR_INT, OUTPUT);
 
  // Initialize the LCD
  lcd.begin(20, 4);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 1);
 
} //end of setup

//Function for Display
void displayTemperature(String label, float currentTemperature, String inputTemperatureStr) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(label);
  lcd.print(" ");
  lcd.print(currentTemperature);

  lcd.setCursor(0, 1);
  lcd.print("Input Temp:");
  lcd.print(inputTemperatureStr);
}


void loop() {
 bool deletePressed = false;
   
 lcd.setCursor(0, 0);
  lcd.print("CurrentTemp:");
  lcd.print(currentTemperature);

  lcd.setCursor(0, 1);
  lcd.print("Input Temp:");
  lcd.print(inputTemperatureStr);

  // Read input from keypad
  char key = keypad.getKey();
 
  if (key != NO_KEY) {
    if (key == 'D') {
        // Set flag to indicate D pressed
       // deletePressed = true;
       // lcd.clear();
       // displayTemperature("CurrentTemp:", currentTemperature, inputTemperatureStr);
       // Set the string to an empty value to delete all characters
    inputTemperatureStr = "";
    digitCount = 0;
    // Update the display
    displayTemperature("CurrentTemp:", currentTemperature, inputTemperatureStr);
 
       
    }
     else if (key >= '0' && key <= '9' && digitCount < MAX_DIGITS) {Serial.print(key);
      if (inputTemperatureStr.length() > 0 && digitCount == 0) {
        inputTemperatureStr = "";
        delay(50);
      }
      // Append the key to the input temperature string
      inputTemperatureStr += key;
      // Increment digit count when a new digit is entered
      digitCount++;
    } else if (key == 'C') {
      // Convert input temp string to int
      int inputTemperature = inputTemperatureStr.toInt();
      // Check if valid range
      if (inputTemperature >= 200 && inputTemperature <= 500) {
        // Display input accepted message on LCD
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Input Accepted");
        delay(1000);
        lcd.clear();
        lcd.setCursor(0, 1);
        //Display
        displayTemperature("CurrentTemp:", currentTemperature, inputTemperatureStr);
       // Clear input temp string
        inputTemperatureStr = "";
        // Reset digit count
        digitCount = 0;
        summer = 1;
        dimmer.setPower(96); // 
      } else {
         // Display error message
    inputTemperatureStr = ""; // Clear invalid input
   
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("REJECTED VALUE");
    lcd.setCursor(0, 1);
    lcd.print("PICK 200F-500F");
    // Delay to display error message
    delay(2500);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("CurrentTemp:");
    lcd.print(currentTemperature);
    lcd.setCursor(0, 1);
    lcd.print("Input Temp:");
    lcd.print(inputTemperatureStr);
    digitCount = 0;
    return; // Return to the beginning of the loop
    }
     
    // START OF SUMMERS LOOP // START OF SUMMERS LOOP // START OF SUMMERS LOOP 
    while (summer == 1){{
    digitalWrite(T0_PIN, LOW);
    digitalWrite(T1_PIN, LOW);
    digitalWrite(T2_PIN, LOW);
    Serial.print("Sensor1 ");
float a = thermocouple.readFahrenheit();
    if (isnan(a)) {
      Serial.println("Thermocouple fault(s) detected!");
      uint8_t e = thermocouple.readError();
      if (e & MAX31855_FAULT_OPEN) Serial.println("FAULT: Thermocouple is open - no connections.");
      if (e & MAX31855_FAULT_SHORT_GND) Serial.println("FAULT: Thermocouple is short-circuited to GND.");
      if (e & MAX31855_FAULT_SHORT_VCC) Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
    } else {
      // Print temperature on both Serial and LCD
      Serial.print("F = ");
      Serial.println(a);
    }

    delay(100);  // Add a delay between readings of each sensor

    digitalWrite(T0_PIN, HIGH);
    digitalWrite(T1_PIN, LOW);
    digitalWrite(T2_PIN, LOW);
    Serial.print("Sensor2 ");
 float b = thermocouple.readFahrenheit();
    if (isnan(b)) {
      Serial.println("Thermocouple fault(s) detected!");
      uint8_t e = thermocouple.readError();
      if (e & MAX31855_FAULT_OPEN) Serial.println("FAULT: Thermocouple is open - no connections.");
      if (e & MAX31855_FAULT_SHORT_GND) Serial.println("FAULT: Thermocouple is short-circuited to GND.");
      if (e & MAX31855_FAULT_SHORT_VCC) Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
    } else {
      // Print temperature on both Serial and LCD
      Serial.print("F = ");
      Serial.println(b);
    }

    delay(100);  // Add a delay between readings of each sensor

    digitalWrite(T0_PIN, LOW);
    digitalWrite(T1_PIN, HIGH);
    digitalWrite(T2_PIN, LOW);
    Serial.print("Sensor3 ");
 float c = thermocouple.readFahrenheit();
    if (isnan(c)) {
      Serial.println("Thermocouple fault(s) detected!");
      uint8_t e = thermocouple.readError();
      if (e & MAX31855_FAULT_OPEN) Serial.println("FAULT: Thermocouple is open - no connections.");
      if (e & MAX31855_FAULT_SHORT_GND) Serial.println("FAULT: Thermocouple is short-circuited to GND.");
      if (e & MAX31855_FAULT_SHORT_VCC) Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
    } else {
      // Print temperature on both Serial and LCD
      Serial.print("F = ");
      Serial.println(c);
    }

    delay(100);  // Add a delay between readings of each sensor

    digitalWrite(T0_PIN, HIGH);
    digitalWrite(T1_PIN, HIGH);
    digitalWrite(T2_PIN, LOW);
    Serial.print("Sensor4 ");
 float d = thermocouple.readFahrenheit();
    if (isnan(d)) {
      Serial.println("Thermocouple fault(s) detected!");
      uint8_t e = thermocouple.readError();
      if (e & MAX31855_FAULT_OPEN) Serial.println("FAULT: Thermocouple is open - no connections.");
      if (e & MAX31855_FAULT_SHORT_GND) Serial.println("FAULT: Thermocouple is short-circuited to GND.");
      if (e & MAX31855_FAULT_SHORT_VCC) Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
    } else {
      // Print temperature on both Serial and LCD
      Serial.print("F = ");
      Serial.println(d);
    }

    delay(100);  // Add a delay between readings of each sensor

    digitalWrite(T0_PIN, LOW);
    digitalWrite(T1_PIN, LOW);
    digitalWrite(T2_PIN, HIGH);
    Serial.print("Sensor5 ");
 float e = thermocouple.readFahrenheit();
    if (isnan(e)) {
      Serial.println("Thermocouple fault(s) detected!");
      uint8_t e = thermocouple.readError();
      if (e & MAX31855_FAULT_OPEN) Serial.println("FAULT: Thermocouple is open - no connections.");
      if (e & MAX31855_FAULT_SHORT_GND) Serial.println("FAULT: Thermocouple is short-circuited to GND.");
      if (e & MAX31855_FAULT_SHORT_VCC) Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
    } else {
      // Print temperature on both Serial and LCD
      Serial.print("F = ");
      Serial.println(e);
    }

    delay(100);  // Add a delay between readings of each sensor

    digitalWrite(T0_PIN, HIGH);
    digitalWrite(T1_PIN, LOW);
    digitalWrite(T2_PIN, HIGH);
    Serial.print("Sensor6 ");
 float f = thermocouple.readFahrenheit();
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
    }

    delay(100);  // Add a delay between readings of each sensor

    digitalWrite(T0_PIN, LOW);
    digitalWrite(T1_PIN, HIGH);
    digitalWrite(T2_PIN, HIGH);
    Serial.print("Sensor7 ");
 float g = thermocouple.readFahrenheit();
    if (isnan(g)) {
      Serial.println("Thermocouple fault(s) detected!");
      uint8_t e = thermocouple.readError();
      if (e & MAX31855_FAULT_OPEN) Serial.println("FAULT: Thermocouple is open - no connections.");
      if (e & MAX31855_FAULT_SHORT_GND) Serial.println("FAULT: Thermocouple is short-circuited to GND.");
      if (e & MAX31855_FAULT_SHORT_VCC) Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
    } else {
      // Print temperature on both Serial and LCD
      Serial.print("F = ");
      Serial.println(g);
    }

    delay(100);  // Add a delay between readings of each sensor

    digitalWrite(T0_PIN, HIGH);
    digitalWrite(T1_PIN, HIGH);
    digitalWrite(T2_PIN, HIGH);
    Serial.print("Sensor8 ");
 float h = thermocouple.readFahrenheit();
    if (isnan(h)) {
      Serial.println("Thermocouple fault(s) detected!");
      uint8_t e = thermocouple.readError();
      if (e & MAX31855_FAULT_OPEN) Serial.println("FAULT: Thermocouple is open - no connections.");
      if (e & MAX31855_FAULT_SHORT_GND) Serial.println("FAULT: Thermocouple is short-circuited to GND.");
      if (e & MAX31855_FAULT_SHORT_VCC) Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
    } else {
      // Print temperature on both Serial and LCD
      Serial.print("F = ");
      Serial.println(h);
    }

    delay(100);  // Add a delay between readings of each sensor

    Serial.print("Sensor9 ");
 float i = thermocouple2.readFahrenheit();
    if (isnan(i)) {
      Serial.println("Thermocouple fault(s) detected!");
      uint8_t e = thermocouple.readError();
      if (e & MAX31855_FAULT_OPEN) Serial.println("FAULT: Thermocouple is open - no connections.");
      if (e & MAX31855_FAULT_SHORT_GND) Serial.println("FAULT: Thermocouple is short-circuited to GND.");
      if (e & MAX31855_FAULT_SHORT_VCC) Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
    } else {
      // Print temperature on both Serial and LCD
      Serial.print("F = ");
      Serial.println(i);
    }

    delay(100);

  float values[] = {a,b,c,d,e,f,g,h,i}; // values
  float weights[] = {1,1,1,1,1,1,1,1,1};   // weights
  int n = sizeof(values) / sizeof(values[0]); // Calculate the number of elements
   
  float weightedSum = 0;
  float totalWeight = 0;

  // Calculate the weighted sum and total weight
  for (int i = 0; i < n; i++) {
    weightedSum += values[i] * weights[i];
    totalWeight += weights[i];
  }

  // Calculate the weighted average
  float weightedAverage = (float)weightedSum / totalWeight;
  float tempDifference = abs(inputTemperature - weightedAverage);

  if(inputTemperature > weightedAverage){
        if(tempDifference => 30){
        dimmer.setPower(96);
      } else if(10 <= tempDifference < 30){
        dimmer.setPower(75);
      } else if(3 < tempDifference < 10){
        dimmer.setPower(50);
      }
  } else {
        if(tempDifference => 20){
        dimmer.setPower(20);
      } else if(10 <= tempDifference < 20){
        dimmer.setPower(35);
      } else if(3 < tempDifference < 10) {
        dimmer.setPower(50);
      }
  }

  // Print the result
  Serial.print("Weighted Average: ");
  Serial.println(weightedAverage);
  currentTemperature = weightedAverage;
  delay(100);
  lcd.setCursor(0, 0);
  lcd.print("CurrentTemp:");
  lcd.print(currentTemperature);
 
  //update LED    
  updateLEDs(inputTemperature, a, b, c, d, e, f, g, h, i);
  char key = keypad.getKey();Serial.println(key); delay(200);

    //INTERRUPT BUTTON
    summer = !digitalRead(CLEAR_INT); // Exiting the while loop
   
   
  delay(200);
          }
        }  
      }
    }
  }

void updateLEDs(int inputTemperature, float a,float b,float c,float d,float e,float f,float g,float h,float i) {

  // Print inputTemperature and each temperature value to Serial Monitor for debugging
  Serial.print("Input Temperature: ");
  Serial.println(inputTemperature);
  Serial.print("Sensor 1 Temperature: ");
  Serial.println(a);
  Serial.print("Sensor 2 Temperature: ");
  Serial.println(b);
  Serial.print("Sensor 3 Temperature: ");
  Serial.println(c);
  Serial.print("Sensor 4 Temperature: ");
  Serial.println(d);
  Serial.print("Sensor 5 Temperature: ");
  Serial.println(e);
  Serial.print("Sensor 6 Temperature: ");
  Serial.println(f);
  Serial.print("Sensor 7 Temperature: ");
  Serial.println(g);
  Serial.print("Sensor 8 Temperature: ");
  Serial.println(h);
  Serial.print("Sensor 9 Temperature: ");
  Serial.println(i);

  // Check if the difference between the inputTemperature and each temperature value
  digitalWrite(LED1, abs(inputTemperature - a) <= 15);
  digitalWrite(LED2, abs(inputTemperature - b) <= 15);
  digitalWrite(LED3, abs(inputTemperature - c) <= 15);
  digitalWrite(LED4, abs(inputTemperature - d) <= 15);
  digitalWrite(LED5, abs(inputTemperature - e) <= 15);
  digitalWrite(LED6, abs(inputTemperature - f) <= 15);
  digitalWrite(LED7, abs(inputTemperature - g) <= 15);
  digitalWrite(LED8, abs(inputTemperature - h) <= 15);
  digitalWrite(LED9, abs(inputTemperature - i) <= 15);
  }
