#include <WiFiClientSecure.h>
#include <Losant.h>


/*Put your SSID & Password*/
const char* WIFI_SSID = "xxx";  // Enter SSID here
const char* WIFI_PASS = "xxx";  //Enter Password herez

// Losant Smart Pump-Control credentials
//const char* LOSANT_DEVICE_ID = "xxx";
//const char* LOSANT_ACCESS_KEY = "xxxe";
//const char* LOSANT_ACCESS_SECRET = "xxx";


// Losant eHealth credentials
const char* LOSANT_DEVICE_ID = "xxx";
const char* LOSANT_ACCESS_KEY = "xxx";
const char* LOSANT_ACCESS_SECRET = "xxx";


//Defining the constant
#define REPORTING_PERIOD_MS     5000
#define threshold 300


//Defining the hardware I/O
#define bleedControl 5
#define alarmControl 18
#define autoAlert 23
const int pressureInput = 34; // select the analog input pin for the pressure transducer;


const int pressureZero = 413/1.51; // analog reading of pressure transducer at 0 psi
const int pressureMax = 3720; // analog reading of pressure transducer at 200 psi
const int pressureTransducerMaxPSI = 200; // psi value of transducer being used
const int baudRate = 115200; // constant integer to set the baud rate for serial monitor
const int sensorReadDelay = 250; // constant integer to set the sensor read delay in milliseconds

float pressureValue = 0; // variable to store the value coming from the pressure transducer
int analogReading = 0;  // variable to store the raw ADC value



//Defining state data
bool bleedState = false;
bool alarmState = false;

//Defining cycle state
uint32_t tsLastReport = 0;
uint8_t timeSinceLastReadgas = 0;

const char* rootCABuff = \
"-----BEGIN CERTIFICATE-----\n" \
"xxx \
"-----END CERTIFICATE-----\n";

WiFiClientSecure wifiClient;
LosantDevice device(LOSANT_DEVICE_ID);


void connect() {

  // Connect to Wifi.
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  wifiClient.setCACert(rootCABuff);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }  

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to Losant.
  Serial.println();
  Serial.print("Connecting to Losant...");

  device.connectSecure(wifiClient, LOSANT_ACCESS_KEY, LOSANT_ACCESS_SECRET);

  while(!device.connected()) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected!");
}

void toggleBleed(){
  bleedState = !bleedState;
  Serial.println(bleedState ? "Devcie ON" : "Device OFF");
  digitalWrite(bleedControl, bleedState ? HIGH : LOW);  
}

void toggleAlarm(){
  alarmState = !alarmState;
  Serial.println(DeviceState ? "Devcie ON" : "Device OFF");
  digitalWrite(alarmControl, alarmState ? HIGH : LOW);  
}

// Called whenever the device receives a command from the Losant platform.
void handleCommand(LosantCommand *command) {
  Serial.println();
  Serial.print("Command received: ");
  Serial.println(command->name);
  if (strcmp(command->name, "Trigger") == 0) {
     toggleBleed();
  }
    if (strcmp(command->name, "alarm") == 0) {
     toggleAlarm();
  }
  
}

// Reconnects if required 
void reconnect(){
 bool toReconnect = false;
  // If the WiFi or HUZZAH is not connected to Losant - we should reconnect
  if(WiFi.status() != WL_CONNECTED || !device.connected()) {
    toReconnect = true;
  }
  if(toReconnect) {
    connect();
  }
}

void setup() {
  Serial.begin(baudRate); // initializes serial communication at set baud rate bits per second
  pinMode(bleedControl, OUTPUT);  
  pinMode(alarmControl, OUTPUT);  
  delay(100);  

  Serial.println("Connecting to ");
  Serial.println(WIFI_SSID);
 
  //connect to your local wi-fi network
  WiFi.begin(WIFI_SSID, WIFI_PASS);
 
  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());
   
  // Wait for serial to initialize.
  while(!Serial) { }

  Serial.println("Device Started");
  Serial.println("-------------------------------------");
  Serial.println("Running INDUSTRIAL IoT");
  Serial.println("-------------------------------------");
  
  // Register the command handler to be called when a command is received, from the Losant platform.
  device.onCommand(&handleCommand);

  connect();
}


void smartAlert() {
  //create an alarm logic for beeping
  digitalWrite(autoAlert, HIGH); //use an if count logic simulate a delay of nth weight
  digitalWrite(autoAlert, LOW);  //use an if count logic simulate a delay of mth weight
}


void report(float PSI,float spread) {
  StaticJsonDocument<500> jsonBuffer;
  JsonObject root = jsonBuffer.to<JsonObject>();
  root["PSI"]       = PSI;
  root["spread"]    = spread;
  device.sendState(root);
  Serial.println("Reported to Losant!");
}

void loop() {
   bool toReconnect = false;
    
    if(WiFi.status() != WL_CONNECTED) {
      Serial.println("Disconnected from WiFi");
      toReconnect = true;
    }
    if(!device.connected()) {
      Serial.println("Disconnected from MQTT");
      toReconnect = true;
    }
    if(toReconnect) {
      connect();
    }
  
    device.loop();   
      if (millis() - tsLastReport > REPORTING_PERIOD_MS){
        analogReading = analogRead(pressureInput); // reads value from input pin and assigns to variable
        pressureValue = ((analogReading - pressureZero) * pressureTransducerMaxPSI) / (pressureMax - pressureZero); // conversion equation to convert analog reading to psi
      
        Serial.print("Pressure: ");
        Serial.print(pressureValue, 1); // prints value from previous line to serial
        Serial.println(" psi"); // prints label to serial
      
        delay(sensorReadDelay); // delay in milliseconds between read values
        if(analogReading > threshold)  smartAlert();
           
        report(pressureValue, analogReading);    
        tsLastReport = millis();
    }
}


/*
Go over the IoT part, send data to cloud
Develop the dashboard using the raw sent data as input
*/
