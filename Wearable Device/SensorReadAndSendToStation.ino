//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Libraries for GPS
#include <TinyGPS++.h>
#define RXD2 16
#define TXD2 17
#define GPS_BAUD 9600
TinyGPSPlus gps;
// Create an instance of the HardwareSerial class for Serial 2
HardwareSerial gpsSerial(2);

//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26
#define BAND 466E6

//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 1
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

//Temperature
    #define analogky 34
    #define digitalky 33


  void setup() {
    //initialize Serial Monitor
    Serial.begin(115200);
    gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RXD2, TXD2);
    Serial.println("Serial 2 started at 9600 baud rate");
    //reset OLED display via software
    pinMode(OLED_RST, OUTPUT);
    digitalWrite(OLED_RST, LOW);
    delay(20);
    digitalWrite(OLED_RST, HIGH);
  
    //initialize OLED
    Wire.begin(OLED_SDA, OLED_SCL);
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0,0);
    display.print("LORA SENDER ");
    display.display();
    
    Serial.println("LoRa Sender Test");
  
    //SPI LoRa pins
    SPI.begin(SCK, MISO, MOSI, SS);
    //setup LoRa transceiver module
    LoRa.setPins(SS, RST, DIO0);
    
    if (!LoRa.begin(BAND)) {
      Serial.println("Starting LoRa failed!");
      while (1);
    }
    LoRa.setSyncWord(0xF3);
    Serial.println("LoRa Initializing OK!");
    display.setCursor(0,10);
    display.print("LoRa Initializing OK!");
    display.display();
    //Temperature
    
    pinMode(digitalky, INPUT);
    delay(1000);
  }
  float readTemperature() {

    int analogValue = analogRead(analogky);  // Read analog value
    int digitalValue = digitalRead(digitalky);  // Read digital value
    
    // Convert analog value to temperature (example conversion, adjust based on your sensor's spec)
    float voltage = analogValue * (3.3 / 4095.0);  // ESP32 has 12-bit ADC, so range is 0-4095
    float temperature = (voltage - 0.5) * 100.0;   // Adjust based on your sensor calibration
    
    // You can use the digital value as an additional check if required
    return temperature;
  }
void senddata(){
while (gpsSerial.available() > 0) {
  gps.encode(gpsSerial.read());
  }
  float temperature = readTemperature();
  Serial.println("Sending packet: ");
  Serial.print("LAT : ");
  Serial.println(gps.location.lat(), 6);
  Serial.print("LONG : "); 
  Serial.println(gps.location.lng(), 6);
  Serial.print("Temperature : ");
  Serial.println(temperature, 1);
  
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print("LAT : ");
  LoRa.println(gps.location.lat(), 6);
  LoRa.print("LONG : "); 
  LoRa.println(gps.location.lng(), 6);
  LoRa.print("Temperature : ");
  LoRa.endPacket();
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("LORA SENDER");
  display.setCursor(0,20);
  display.setTextSize(1);
  display.print("Sent data: ");
  display.print("LAT ");
  display.println(gps.location.lat(), 1);
  display.print("LONG ");
  display.println(gps.location.lng(), 1);
  display.setCursor(0,40);
  display.setTextSize(1);
  display.print("Temperature ");
  display.println(temperature, 1);
  display.display();
  
}
void loop() {
  senddata();
  delay(2000);
}
