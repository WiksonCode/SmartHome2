/*
 * 
 * Project made by Viktor Lakcevic 2021
 *          OS "Savo Pejanovic"
 * 
 * 
 */


 /*
  * 
  * How to connect RFID (right to left)
  * 3.3V - 3.3V
  * RST - 2
  * GND - GND
  * EMPTY - /
  * IMI - 50
  * MOS - 51
  * SCK - 52
  * SDA - 53
  * 
  */
 
#include <SPI.h>
#include <MFRC522.h>
#include "Servo.h" 
#include <dht.h>
#include <SoftwareSerial.h>

#define SS_PIN 53  // SS Pin za RFID 
#define RST_PIN 2  // Reset Pin za RFID
#define DC_MOTOR 5 // Motor koji radi kao ventilator ///////////////////////////////////// PROMIJENMITI
#define BUZZER 7
#define LED 4  // Led lampica koja se pali kada su vrata otkljucana
#define DHT11_PIN 3  // Pin na kojem je senzor za temperaturu
#define SERVO 6 // Pin na kojem je servo motor
#define LED2 8

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance. 
Servo servo;
SoftwareSerial hc06(10, 11);   // Pinovi na koje je bluetooth povezan HC-06

dht DHT;

int aplikacija = 0;
String cmd="";

void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication
  hc06.begin(9600);
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC52
  Serial.println("Scan your RFID card...");
  Serial.println();
  servo.attach(SERVO); // Pin na kojem je Servo Motor
  pinMode(LED,OUTPUT);
  pinMode(DC_MOTOR,OUTPUT);
  pinMode(BUZZER,OUTPUT);
  pinMode(LED2,OUTPUT);
  //servo.write(180);
}
void loop() 
{
  int chk = DHT.read11(DHT11_PIN);
  //Serial.println("Temperature = ");
  Serial.println(DHT.temperature);
  //hc06.print(DHT.temperature);
  hc06.print("1");
  Serial.print("Humidity = ");
  Serial.println(DHT.humidity);
  //hc06.print(DHT.humidity);
  delay(1000);


  // CITANJE PODATAKA POSLANIH SA APLIKACIJE

   //Read data from HC06
  while(hc06.available()>0){
    cmd+=(char)hc06.read();
  }

  // ODREDJIVANE KOJE SU KOMANDE
  if(cmd!=""){
    Serial.print("Command recieved : ");
    Serial.println(cmd);
    // We expect ON or OFF from bluetooth
    if(cmd=="ON"){
        digitalWrite(DC_MOTOR,HIGH);
        Serial.println("MOTOR RADI");
        aplikacija = 0;
    }else if(cmd=="OFF"){
        digitalWrite(DC_MOTOR,LOW);
        aplikacija = 1;
    }else if(cmd=="LOCK"){
      servo.write(110);
      digitalWrite(LED,HIGH);
      ringBell();
      digitalWrite(LED,LOW);
      Serial.println("LOCK APP");
      
    }else if(cmd=="UNLOCK"){
      servo.write(40);
      digitalWrite(LED,HIGH);
      ringBell();
      digitalWrite(LED,LOW);
      Serial.println("UNLOCK APP");
    }else if (cmd=="LEDON"){
      digitalWrite(LED2,HIGH);
    }else if (cmd=="LEDOFF"){
      digitalWrite(LED2,LOW);
    }
    cmd=""; //reset cmd
  }
  



  
  // PALJENJE VENTILATORA DC MOTORA PREKO TEMPERATURE
  
  if (DHT.temperature > 29){
    if (aplikacija == 0){
      digitalWrite(DC_MOTOR,HIGH);
    }
  }else{
    if (aplikacija == 1){
       digitalWrite(DC_MOTOR,LOW);
    }
  }

    // PROVJERA RFID KARTICE
    
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "66 76 3F 29") //change here the UID of the card/cards that you want to give access
  {

       ZakljuvanjeRFID();
    
    
    
  }
 
 else   {
    Serial.println(" Odbijem pristup");
    delay(3000);
  }
} 


void ZakljuvanjeRFID(){
      Serial.println("Odobren pristup");
      digitalWrite(LED,HIGH);  // Otkljucavanje
      ringBell();
      Serial.println("Otkljucavanje");
      servo.write(40); // 90 stepeni je zakljucavanje
      Serial.println("Vrata se zakljucavaju za 10 sekundi");
      delay(10000);
      digitalWrite(LED,LOW);
      Serial.println("Zakljucavanje");
      servo.write(110);
      
}

void ringBell(){ //plays a tone on the piezo buzzer
    
    tone(BUZZER,2500,1000);
    delay(1000);
    tone(BUZZER,1000,2000);
    delay(2000);
    digitalWrite(BUZZER,LOW);
    delay(2000);
    
  
   
}
