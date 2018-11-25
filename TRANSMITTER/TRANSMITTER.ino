/*
* Arduino Wireless Communication Tutorial
*     Example 1 - Transmitter Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#define trigPin 10
#define echoPin 9
#define redPin 5
#define grnPin 6
int a = 1;

RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";
void setup()
{
  pinMode(redPin, OUTPUT);
  pinMode(grnPin, OUTPUT);
  digitalWrite(redPin, HIGH);
  digitalWrite(grnPin, HIGH);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(grnPin, HIGH);
  
  delay(500);

  digitalWrite(redPin, LOW);
}

void loop() 
{
   long duration, distance;
   int trig = 0;
 /*  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration*340/20000;
    digitalWrite(grnPin, LOW);*/

  distance = 13597/analogRead(A0);
 
  if(distance > 50)
  {
    trig = 0;
    
    a = -a;
    digitalWrite(redPin, LOW);
    if(a == 1)
  {
    digitalWrite(grnPin, HIGH);
  }
  else if(a == -1)
  {
    digitalWrite(grnPin, LOW);
  }
  }

  if(distance < 50)
  {
    digitalWrite(redPin, HIGH);
    digitalWrite(grnPin, LOW);

    trig = 5000;
  }

  distance += trig;
  radio.write(&distance, sizeof(long));
  Serial.print(distance);
  Serial.println(" cm");

  delay(500);
}
