const int pressureInput = 34; // select the analog input pin for the pressure transducer
const int pressureZero = 413/1.51; // analog reading of pressure transducer at 0 psi
const int pressureMax = 3720; // analog reading of pressure transducer at 200 psi
const int pressureTransducerMaxPSI = 200; // psi value of transducer being used
const int baudRate = 115200; // constant integer to set the baud rate for serial monitor
const int sensorReadDelay = 250; // constant integer to set the sensor read delay in milliseconds

float pressureValue = 0; // variable to store the value coming from the pressure transducer
int analogReading = 0;  // variable to store the raw ADC value


void setup() {
  Serial.begin(baudRate); // initializes serial communication at set baud rate bits per second
}

void loop() {
  analogReading = analogRead(pressureInput); // reads value from input pin and assigns to variable
  pressureValue = ((analogReading - pressureZero) * pressureTransducerMaxPSI) / (pressureMax - pressureZero); // conversion equation to convert analog reading to psi

  Serial.print("Pressure: ");
  Serial.print(pressureValue, 1); // prints value from previous line to serial
  Serial.println(" psi"); // prints label to serial
  Serial.print(analogReading);
  Serial.println(" Analog value");

  delay(sensorReadDelay); // delay in milliseconds between read values
}
