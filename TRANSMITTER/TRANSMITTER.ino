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
int keep = 0;

struct dataPack
{
  int isActivate;
  int distance;
};

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
  digitalWrite(grnPin, HIGH);
  
  delay(500);

  digitalWrite(redPin, LOW);
}

void loop() 
{
  struct dataPack data;
  data.isActivate = 1;

  data.distance = 13597/analogRead(A0);
 
  if(data.distance > 50)
  {    
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
    
    keep = 0;
  }

  if(data.distance < 50)
  {
    digitalWrite(redPin, HIGH);
    digitalWrite(grnPin, LOW);
    
    keep += 1;

    if(keep >= 6)
    {
      data.isActivate = -1;
    }
  }

  radio.write(&data, sizeof(data));
  Serial.print(data.distance);
  Serial.println(" cm");

  delay(500);
}
