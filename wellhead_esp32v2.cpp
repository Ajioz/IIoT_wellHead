#include "Wire.h" // allows communication over I2C devices
#include "LiquidCrystal_I2C.h" // allows interfacing with LCD screens

const int pressureInput = A0; // select the analog input pin for the pressure transducer
const int pressureZero = 413; // analog reading of pressure transducer at 0 psi
const int pressureMax = 3720; // analog reading of pressure transducer at 200 psi
const int pressureTransducerMaxPSI = 200; // psi value of transducer being used
const int baudRate = 115200; // constant integer to set the baud rate for serial monitor
const int sensorReadDelay = 250; // constant integer to set the sensor read delay in milliseconds

float pressureValue = 0; // variable to store the value coming from the pressure transducer
int analogReading = 0;  // variable to store the raw ADC value

LiquidCrystal_I2C lcd(0x3f, 20, 4); // sets the LCD I2C communication address; format(address, columns, rows)

void setup() {
  Serial.begin(baudRate); // initializes serial communication at set baud rate bits per second
  lcd.begin();           // initializes the LCD screen
  lcd.backlight();       // turn on LCD backlight
}

void loop() {
  analogReading = analogRead(pressureInput); // reads value from input pin and assigns to variable
  pressureValue = ((analogReading - pressureZero) * pressureTransducerMaxPSI) / (pressureMax - pressureZero); // conversion equation to convert analog reading to psi

  Serial.print("Pressure: ");
  Serial.print(pressureValue, 1); // prints value from previous line to serial
  Serial.println(" psi"); // prints label to serial

  lcd.setCursor(0, 0); // sets cursor to column 0, row 0
  lcd.print("Pressure:"); // prints label
  lcd.print(pressureValue, 1); // prints pressure value to LCD screen, 1 digit on float
  lcd.print(" psi"); // prints label after value
  lcd.print("   "); // to clear the display after large values or negatives

  delay(sensorReadDelay); // delay in milliseconds between read values
}
