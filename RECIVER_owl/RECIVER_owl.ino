
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

const byte address[6] = "23470";

void stepMove(int a)
{
  digitalWrite(STEPDIR, LOW);
  for(int i = 0; i < a; i++)
  {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(1000);
    digitalWrite(STEP, LOW);
    delayMicroseconds(2000);
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


void trigger(bool a)
{
  
  if(g_trigger_stat == a)
    return;

   g_trigger_stat = a;
  
  if(a)
  {
    stepMove(10);
    digitalWrite(LED, HIGH);
    digitalWrite(relay, HIGH);
    sendCommand(CMD_PLAY, 0);
    stepMove(100);
  }
  else
  {
    digitalWrite(LED, LOW);
    digitalWrite(relay, LOW);
    sendCommand(CMD_PAUSE, 0);
    sendCommand(CMD_RESET, 0);
  }
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
  
  digitalWrite(LED, LOW);

  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  
  display.initialize();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("LOADING...");
  display.update();
  
  delay(500);
}
void loop() {
  
  if (radio.available())
  {
    radio.read(&data, sizeof(data));
    Serial.println(data.distance);
    
    if(data.isActivate == -1)
    {
      trigger(true);
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.println("ACTIVATED");  
      display.setCursor(0,16);
      display.print(data.distance);
      display.println("CM");
      display.setCursor(0,32);
      display.print(data.distance1);
      display.println("CM");
      display.update();
      display.clear();
    }
    else
    {
      trigger(false);
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.println("DISTANCE:");  
      display.setCursor(0,16);
      display.print(data.distance);
      display.println("CM");
      display.setCursor(0,32);
      display.print(data.distance1);
      display.println("CM");
      display.update();
      display.update();
      display.clear();
    }
    
  }
  
  if(digitalRead(button) == LOW || data.isActivate == -1)
  { 
    trigger(true);
  }
  else
  { 
     trigger(false);
  }
  
}
