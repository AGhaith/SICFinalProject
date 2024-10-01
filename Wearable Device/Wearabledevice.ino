#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>
#include <LoRa.h>
#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C 
#define ss 5
#define rst 14
#define dio0 2
#define RT0 10000  // Resistance at 25°C
#define B 3977     // Beta coefficient for temp
#define VCC 3.3    // ESP32 operating voltage (3.3V)
#define R 10000  
#define UV_SENSOR_PIN 35  // ESP32 Analog Pin (ADC1_7)
MAX30105 particleSensor;// Resistor value (10K ohms)
#define led 15
static const int RXPin = 4, TXPin = 0;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial softser(RXPin, TXPin);
//MAX30105 particleSensor;// Resistor value (10K ohms)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
int uvReading = 0;
const float voltageToIntensityFactor = 307.69;
///////////////////////////// Variables
float RT, VR, ln, T, t0, VRT;  //for temprature 
///////////////////////////////////////////////
const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;
long interval=5000;
long lastsend=0 ;
void setup() {
  Serial.begin(115200);  // Higher baud rate for ESP32
  softser.begin(GPSBaud);
  mySwitch.enableReceive(27);
    pinMode(led, OUTPUT);

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  t0 = 25 + 273.15;      // Reference temperature in Kelvin
  LoRa.setPins(ss, rst, dio0);
  
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("Connecting");  // will be replaced
  display.display();

  Serial.println("Starting LoRa");
  if (!LoRa.begin(433E6)) {
  Serial.println("Starting LoRa failed!");
   display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("WAIT");  // will be replaced
  display.display();
  pinMode(UV_SENSOR_PIN, INPUT);
  while (1);
  }
 display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("CONNECTED");  // will be replaced
  display.display();
  pinMode(UV_SENSOR_PIN, INPUT);

  // Initialize sensor heart sensor 
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
    {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
    }
  Serial.println("Place your index finger on the sensor with steady pressure.");
  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
}

float temp(){
  VRT = analogRead(34);  // Read analog value from GPIO34 (or whichever pin you're using) GPIO ADC1_6
  VRT = (VCC / 4095.0) * VRT;  // Conversion to voltage for ESP32 (12-bit ADC)
  VR = VCC - VRT;
  RT = VRT / (VR / R);          // Resistance of thermistor
  ln = log(RT / RT0);
  T = (1 / ((ln / B) + (1 / t0)));  // Temperature in Kelvin
  T = T - 273.15;  
  LoRa.beginPacket();                   // start packet           // add sender address
  LoRa.print("Temperature : ");
  LoRa.print(T);
  LoRa.endPacket();    
  Serial.print("Temperature: ");
  Serial.print(T);  // Celsius
  Serial.print(" °C | ");
  return  (T);
}
float uv(){
  uvReading = analogRead(UV_SENSOR_PIN);
  // Convert the analog reading (0–4095) to voltage (0–3.3V)
  float voltage = uvReading * 3.3 / 4095.0;
  // Calculate the UV intensity in mW/m² using the conversion factor
  float uvIntensity = voltage * voltageToIntensityFactor;
  LoRa.beginPacket();                   // start packet           // add sender address
  LoRa.print("UltraViolet : ");
  LoRa.print(uvIntensity, 2);  
  LoRa.endPacket();   
  Serial.print("UV Voltage: ");
  Serial.print(voltage, 3);  // Print voltage with 3 decimal places
  Serial.print(" V, UV Intensity: ");
  Serial.print(uvIntensity, 2);  
  Serial.println(" mW/m²");
  return (uvIntensity, 2);
}
void loop() { 
  if (mySwitch.available()) {
    Serial.print("Received ");
    Serial.print( mySwitch.getReceivedValue() );
    digitalWrite(led, LOW);
    mySwitch.resetAvailable();
  }
  else {
    digitalWrite(led, HIGH);
  }
  

  if (millis()-lastsend >= interval){
  if (softser.available() > 0) {
    gps.encode(softser.read());
    if (gps.location.isUpdated()) {
      display.clearDisplay();
      display.setTextColor(SSD1306_WHITE);
      display.setTextSize(1);
      display.setCursor(0, 5);
      display.print("Time: ");
      display.setCursor(30, 5);
      display.print(gps.time.hour() + 3);
      display.setCursor(42, 5);
      display.print(":");
      display.setCursor(47, 5);
      display.print(gps.time.minute());
      display.setCursor(60, 5);
      display.print(":");
      display.setCursor(66, 5);
      display.print(gps.time.second());
      display.setCursor(0, 18);
      display.print("Lat: ");
      display.setCursor(25, 18);
      display.print(gps.location.lat(), 5);
      display.setCursor(0, 33);
      display.print("Lng: ");
      display.setCursor(25, 33);
      display.print(gps.location.lng(), 5);
      display.display();
      LoRa.beginPacket();                   // start packet           // add sender address
      LoRa.print("LAT : ");
      LoRa.print(gps.location.lat(),5);                 // add payload
      LoRa.endPacket();   
      LoRa.beginPacket();                   // start packet           // add sender addres
      LoRa.print("LNG : ");
      LoRa.print(gps.location.lng(),5);                 // add payload
      LoRa.endPacket();
      }
    }
    temp();
    uv();
    lastsend=millis() ; 

    }else {
      while(millis()-lastsend <= interval){
      long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

        LoRa.beginPacket();                   // start packet           // add sender address
        LoRa.print("BPM : ");
        LoRa.print(beatAvg);                 // add payload
        LoRa.endPacket();   
        Serial.print("BPM : ");
        Serial.print(beatsPerMinute);
        Serial.print(", Avg BPM=");
        Serial.println(beatAvg);   

  if (irValue < 50000)
    Serial.print(" No finger?");

  Serial.println();


      }
    }
    }
    
    


void sendMessage(String outgoing) {
  LoRa.beginPacket();                   // start packet
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
}
