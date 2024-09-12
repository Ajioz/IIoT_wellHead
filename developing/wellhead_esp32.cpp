#include "Wire.h" //allows communication over I2C devices
#include "LiquidCrystal_I2C.h" //allows interfacing with LCD screens

const int pressureInput = A0; // Select the analog input pin for the pressure transducer
const float pressureZeroVoltage = 0.333; // Voltage corresponding to 0 psi after the voltage divider
const float pressureMaxVoltage = 3.0; // Voltage corresponding to 200 psi after the voltage divider
const int pressureTransducerMaxPSI = 200; // Maximum psi value of the transducer
const int sensorReadDelay = 250; // Delay in milliseconds between sensor reads

float pressureValue = 0; // Variable to store the converted pressure value
int analogReading = 0;   // Variable to store the raw ADC value

LiquidCrystal_I2C lcd(0x3f, 20, 4); // Sets the LCD I2C communication address; format(address, columns, rows)

void setup() {
  Serial.begin(115200); // Initializes serial communication
  lcd.begin();          // Initializes the LCD screen
  lcd.backlight();      // Turn on LCD backlight
}

void loop() {
  // Read the analog input from the sensor
  analogReading = analogRead(pressureInput); // Read the ADC value from the ESP32 (0 to 4095)

  // Convert the analog reading to a voltage (ESP32 12-bit resolution, 0 to 3.3V range)
  float sensorVoltage = (analogReading / 4095.0) * 3.3;

  // Convert the voltage to pressure (psi)
  pressureValue = ((sensorVoltage - pressureZeroVoltage) * pressureTransducerMaxPSI) / (pressureMaxVoltage - pressureZeroVoltage);

  // Print the pressure value to the Serial Monitor
  Serial.print("Pressure: ");
  Serial.print(pressureValue, 1); // 1 decimal precision
  Serial.println(" psi");

  // Display the pressure on the LCD
  lcd.setCursor(0, 0);          // Set cursor to column 0, row 0
  lcd.print("Pressure: ");       // Display label
  lcd.print(pressureValue, 1);   // Display pressure value with 1 decimal point
  lcd.print(" psi");             // Display unit
  
  delay(sensorReadDelay); // Delay between readings
}
