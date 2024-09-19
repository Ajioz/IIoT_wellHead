#include <WiFiClientSecure.h>
#include <Losant.h>


/*Put your SSID & Password*/
const char* WIFI_SSID = "AjioziDev";  // Enter SSID here
const char* WIFI_PASS = "sandiewhyte123";  //Enter Password herez

// Losant Smart Pump-Control credentials
//const char* LOSANT_DEVICE_ID = "63f4bd0487b0e03668133368";
//const char* LOSANT_ACCESS_KEY = "0f05bc18-14a5-441d-8052-9844d06247be";
//const char* LOSANT_ACCESS_SECRET = "897e12416f5abe5233b8b617bb77e81c054c19197860d2f5a9660e5d2abaf570";


// Losant eHealth credentials
const char* LOSANT_DEVICE_ID = "624020db4924f62857bc4695";
const char* LOSANT_ACCESS_KEY = "2fe39268-bea5-4141-9b73-11b7d80fce30";
const char* LOSANT_ACCESS_SECRET = "f6c21976af3c0d2de00a294b9a3cc1183fe5ececca1cfd6f08df2f2ca06d3310";


//Defining the constant
#define REPORTING_PERIOD_MS     5000
#define threshold 300


//Defining the hardware I/O
#define deviceControl 2
const int pressureInput = 34; // select the analog input pin for the pressure transducer;


const int pressureZero = 413/1.51; // analog reading of pressure transducer at 0 psi
const int pressureMax = 3720; // analog reading of pressure transducer at 200 psi
const int pressureTransducerMaxPSI = 200; // psi value of transducer being used
const int baudRate = 115200; // constant integer to set the baud rate for serial monitor
const int sensorReadDelay = 250; // constant integer to set the sensor read delay in milliseconds

float pressureValue = 0; // variable to store the value coming from the pressure transducer
int analogReading = 0;  // variable to store the raw ADC value



//Defining state data
bool DeviceState = false;

//Defining cycle state
uint32_t tsLastReport = 0;
uint8_t timeSinceLastReadgas = 0;

const char* rootCABuff = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
"QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n" \
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n" \
"CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n" \
"nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n" \
"43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n" \
"T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n" \
"gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n" \
"BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n" \
"TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n" \
"DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n" \
"hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n" \
"06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n" \
"PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n" \
"YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n" \
"CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n" \
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

void toggle(){
  DeviceState = !DeviceState;
  Serial.println(DeviceState ? "Devcie ON" : "Device OFF");
  digitalWrite(deviceControl, DeviceState ? HIGH : LOW);  
}

// Called whenever the device receives a command from the Losant platform.
void handleCommand(LosantCommand *command) {
  Serial.println();
  Serial.print("Command received: ");
  Serial.println(command->name);
  if (strcmp(command->name, "Trigger") == 0) {
     toggle();
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
  pinMode(deviceControl, OUTPUT);  
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
  Serial.println("Running INDUSTRIAL IIOT");
  Serial.println("-------------------------------------");
  
  // Register the command handler to be called when a command is received, from the Losant platform.
  device.onCommand(&handleCommand);

  connect();
}


void autoAlert() {
  digitalWrite(deviceControl, LOW); 
  Serial.println(deviceControl);
}


void report(float LPGas,float CO, float Smoke, float Hydrogen) {
  StaticJsonDocument<500> jsonBuffer;
  JsonObject root = jsonBuffer.to<JsonObject>();
  root["LPGas"]     = LPGas;
  root["COgas"]     = COgas;
  root["Smoke"]     = Smoke;
  root["Hydrogen"]  = Hydrogen;
  device.sendState(root);
  Serial.println("Reported to Losant!");
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


/*
Go over the IoT part, send data to cloud
Develop the dashboard using the raw sent data as input
*/
