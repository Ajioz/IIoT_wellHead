#include "Wire.h" //allows communication over i2c devices
#include "LiquidCrystal_I2C.h" //allows interfacing with LCD screens

const int pressureInput = A0; //select the analog input pin for the pressure transducer
const float pressureZero = 102.4; //analog reading of pressure transducer at 0 psi (0.5V)
const float pressureMax = 921.6; //analog reading of pressure transducer at 200 psi (4.5V)
const int pressureTransducerMaxPSI = 200; //psi value of transducer being used
const int baudRate = 9600; //constant integer to set the baud rate for serial monitor
const int sensorReadDelay = 250; //constant integer to set the sensor read delay in milliseconds

float pressureValue = 0; //variable to store the value coming from the pressure transducer

LiquidCrystal_I2C lcd(0x3f, 20, 4); //sets the LCD I2C communication address; format(address, columns, rows)

void setup() //setup routine, runs once when system turned on or reset
{
  Serial.begin(baudRate); //initializes serial communication at set baud rate bits per second
  lcd.begin(); //initializes the LCD screen
}

void loop() //loop routine runs over and over again forever
{
  pressureValue = analogRead(pressureInput); //reads value from input pin and assigns to variable
  // Conversion equation to convert analog reading to psi
  pressureValue = ((pressureValue - pressureZero) * pressureTransducerMaxPSI) / (pressureMax - pressureZero);
  
  Serial.print(pressureValue, 1); //prints value from previous line to serial
  Serial.println(" psi"); //prints label to serial

  lcd.setCursor(0, 0); //sets cursor to column 0, row 0
  lcd.print("Pressure: "); //prints label
  lcd.print(pressureValue, 1); //prints pressure value to LCD screen, 1 digit on float
  lcd.print(" psi"); //prints label after value
  lcd.print("   "); //clears display after large values or negatives
  
  delay(sensorReadDelay); //delay in milliseconds between read values
}
