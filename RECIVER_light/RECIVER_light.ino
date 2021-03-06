
#include <SoftwareSerial.h>

#define ARDUINO_RX 5  //should connect to TX of the Serial MP3 Player module
#define ARDUINO_TX 6  //connect to RX of the module

SoftwareSerial mp3(ARDUINO_RX, ARDUINO_TX);

static int8_t Send_buf[8] = {0}; 
static uint8_t ansbuf[10] = {0}; 

static bool g_trigger_stat;

String mp3Answer;         

struct dataPack
{
  int isActivate;
  int distance;
  int distance1;
};
struct dataPack data;
bool temmie = false;
int sans = 0;


/********defines********/

#define CMD_NEXT_SONG     0X01  
#define CMD_PREV_SONG     0X02  
#define CMD_PLAY_W_INDEX  0X03
#define CMD_VOLUME_UP     0X04
#define CMD_VOLUME_DOWN   0X05
#define CMD_SET_VOLUME    0X06

#define CMD_SNG_CYCL_PLAY 0X08  
#define CMD_SEL_DEV       0X09
#define CMD_SLEEP_MODE    0X0A
#define CMD_WAKE_UP       0X0B
#define CMD_RESET         0X0C
#define CMD_PLAY          0X0D
#define CMD_PAUSE         0X0E
#define CMD_PLAY_FOLDER_FILE 0X0F

#define CMD_STOP_PLAY     0X16
#define CMD_FOLDER_CYCLE  0X17
#define CMD_SHUFFLE_PLAY  0x18 
#define CMD_SET_SNGL_CYCL 0X19 

#define CMD_SET_DAC 0X1A
#define DAC_ON  0X00
#define DAC_OFF 0X01

#define CMD_PLAY_W_VOL    0X22
#define CMD_PLAYING_N     0x4C
#define CMD_QUERY_STATUS      0x42
#define CMD_QUERY_VOLUME      0x43
#define CMD_QUERY_FLDR_TRACKS 0x4e
#define CMD_QUERY_TOT_TRACKS  0x48
#define CMD_QUERY_FLDR_COUNT  0x4f

/************ Opitons ***********/

#define DEV_TF            0X02
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_ssd1306syp.h>
#define SDA_PIN A4
#define SCL_PIN A5
Adafruit_ssd1306syp display(SDA_PIN,SCL_PIN);
RF24 radio(7, 8); // CE, CSN
#define LED 10
#define button 4
#define relay 9
#define STEPDIR 2
#define STEP 3
#define redPin 5
#define grnPin 6

const byte address[6] = "23470";

void stepMove(int a, bool dir)
{
  digitalWrite(STEPDIR, dir);
  for(int i = 0; i < a; i++)
  {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(500);
    digitalWrite(STEP, LOW);
    delayMicroseconds(500);
  }
}

void sendCommand(int8_t command, int16_t dat)
{
  delay(20);
  Send_buf[0] = 0x7e;   //
  Send_buf[1] = 0xff;   //
  Send_buf[2] = 0x06;   // Len
  Send_buf[3] = command;//
  Send_buf[4] = 0x01;   // 0x00 NO, 0x01 feedback
  Send_buf[5] = (int8_t)(dat >> 8);  //datah
  Send_buf[6] = (int8_t)(dat);       //datal
  Send_buf[7] = 0xef;   //
  Serial.print("Sending: ");
  for (uint8_t i = 0; i < 8; i++)
  {
    mp3.write(Send_buf[i]) ;
    Serial.print(sbyte2hex(Send_buf[i]));
  }
  Serial.println();

}
/********stuff*********/

String sbyte2hex(uint8_t b)
{
  String shex;

  shex = "0X";

  if (b < 16) shex += "0";
  shex += String(b, HEX);
  shex += " ";
  return shex;
}

/********sawnser********/

String sanswer(void)
{
  uint8_t i = 0;
  String mp3answer = "";

  // Get only 10 Bytes
  while (mp3.available() && (i < 10))
  {
    uint8_t b = mp3.read();
    ansbuf[i] = b;
    i++;

    mp3answer += sbyte2hex(b);
  }

  // if the answer format is correct.
  if ((ansbuf[0] == 0x7E) && (ansbuf[9] == 0xEF))
  {
    return mp3answer;
  }

  return "???: " + mp3answer;
}

/**************code - setup************/

void stickHeadUp()
{
  stepMove(700, LOW);

  digitalWrite(A2, HIGH);
  delay(200);
  digitalWrite(A2, LOW);
  delay(200);
  digitalWrite(A2, HIGH);
  delay(200);
  digitalWrite(A2, LOW);
  delay(200);
  digitalWrite(A2, HIGH);
  delay(200);
  digitalWrite(A2, LOW);
  delay(200);
  digitalWrite(A2, HIGH);
  delay(200);
  digitalWrite(A2, LOW);
  delay(200);
  digitalWrite(A2, HIGH);
  delay(200);
  digitalWrite(A2, LOW);
  delay(200);
  digitalWrite(A2, HIGH);
  delay(200);
  digitalWrite(A2, LOW);
  delay(200);

  stepMove(700, HIGH);
}

void setup() {
  data.isActivate = 0;
  Serial.begin(9600);
  mp3.begin(9600);
  radio.begin();
  
  sendCommand(CMD_SEL_DEV, DEV_TF);
  delay(500);
  pinMode(LED, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(relay, OUTPUT);
  pinMode(STEPDIR, OUTPUT);
  pinMode(STEP, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(grnPin, OUTPUT);
  
  digitalWrite(LED, LOW);

  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening(); 
  digitalWrite(grnPin, LOW);
  digitalWrite(redPin, LOW);
}
void loop() {
  if (radio.available())
  {
    radio.read(&data, sizeof(data));
    Serial.println(data.distance);

    if(data.isActivate != -1)
    {
        sans = 0;
        digitalWrite(redPin, LOW);
    }
       
    if(data.isActivate == -1 && sans < 2)
    {
      sans++;
      digitalWrite(redPin, LOW);
    }
    else if(sans == 2)
    {
      digitalWrite(redPin, HIGH);
      delay(100);
      digitalWrite(redPin, LOW);
      delay(150);
      digitalWrite(redPin, HIGH);
      delay(100);
      digitalWrite(redPin, LOW);
      delay(50);
      digitalWrite(redPin, HIGH);
      delay(100);
      digitalWrite(redPin, LOW);
      delay(150);
      digitalWrite(redPin, HIGH);
      delay(100);
      sans = 255;
      while(sans--)
      {
        analogWrite(redPin, sans);
        delay(3);
      }
      delay(1000);
      digitalWrite(redPin, LOW);
      delay(100);
      digitalWrite(redPin, HIGH);
      sans = 255;
      while(sans--)
      {
        analogWrite(redPin, sans);
        delay(2);
      }
      delay(100);
      digitalWrite(redPin, LOW);
      delay(100);
      digitalWrite(redPin, LOW);
      delay(150);
      digitalWrite(redPin, HIGH);
      delay(100);
      digitalWrite(redPin, LOW);
      
      
      delay(100);
      digitalWrite(redPin, LOW);
      delay(1000);
      digitalWrite(redPin, HIGH);
      delay(500);
      digitalWrite(redPin, LOW);
      delay(4000);
      digitalWrite(redPin, HIGH);
      sans = 255;
      while(sans--)
      {
        analogWrite(redPin, sans);
        delay(7);
      }
      delay(400);
      digitalWrite(redPin, LOW);
      delay(600);
      digitalWrite(redPin, HIGH);
      delay(200);
      digitalWrite(redPin, LOW);
      
      delay(200);
      
      digitalWrite(redPin, HIGH);
      delay(100);
      digitalWrite(redPin, LOW);
      delay(200);
      digitalWrite(redPin, HIGH);
      delay(100);
      digitalWrite(redPin, LOW);
      delay(50);
      digitalWrite(redPin, HIGH);
      delay(100);
      digitalWrite(redPin, LOW);
      delay(5000);
     
    }
    
  }
 
}
