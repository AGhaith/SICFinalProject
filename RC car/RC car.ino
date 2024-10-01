#include <LoRa.h>
int IN1 = 26; 
int IN2 = 25; 
int IN3 = 33;
int IN4 = 32;
int ena =2;
int enb =4;
TaskHandle_t Task0;
int restart = 0;
void setup() {
  Serial.begin(115200);
  #define ss 13
  #define rst 14
  #define dio0 12
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ena, OUTPUT);
  pinMode(enb, OUTPUT);
  digitalWrite(ena,HIGH);
  digitalWrite(enb,HIGH);
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) { 
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  else {
        Serial.println("Lora initialized");

  }
  analogWrite(ena, 100);
  analogWrite(enb, 100);
}

void loop() {
  int packetSize = LoRa.parsePacket();
  onReceive(packetSize);
  //send gps
}
void forward(){
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
}
void backward(){
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,HIGH);
}
void right(){

  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);

}
void left(){

  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);

}
void stop(){

  if (restart > 10){

 LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) { 
    Serial.println("Starting LoRa failed!");
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,LOW);
    digitalWrite(IN3,LOW);
    digitalWrite(IN4,LOW);
    while (1);
  }
  else {
        Serial.println("Lora initialized");
  }
  }
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
  restart++;

}
void onReceive(int packetSize) {
  int size = (int)packetSize;
  if (size == 0) return;

  String LoRaData = LoRa.readString();
  Serial.print('\'');
  Serial.print(LoRaData);
  Serial.print("' with RSSI ");
  Serial.println(LoRa.packetRssi());
  
  if (LoRaData == "F"){
    forward();
  }
  else if (LoRaData == "B"){
    backward();
  }
  else if (LoRaData == "R"){
    right();
  }
  else if (LoRaData == "L"){
    left();
  }
  else if (LoRaData == "S"){
    stop();
  }
  
}