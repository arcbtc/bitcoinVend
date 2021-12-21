
/**
 *  PIN MAP for ESP32 NODEMCU-32S, other ESP32 dev boards will vary
 *  Keypad (12-32
 *  1.8 128/160 TFT PIN MAP: [VCC - 5V, GND - GND, CS - GPIO5, Reset - GPIO16, AO (DC) - GPI17, SDA (MOSI) - GPIO23, SCK - GPIO18, LED - 3.3V]
 */
 
#include "SPI.h"
//#include "TFT_eSPI.h"
#include <Keypad.h>
#include <string.h>
#include "qrcode.h"
#include "Bitcoin.h"
#include <Base64.h>
#include <Hash.h>
#include <Conversion.h>
#include <math.h>
#include <TFT_eSPI.h>

////////////////////////////////////////////////////////
////////CHANGE! USE LNURLPoS EXTENSION IN LNBITS////////
////////////////////////////////////////////////////////

//Server details
String baseURL = "https://legend.lnbits.com/lnurlpos/api/v1/lnurl/Menv6Rx6Cpnd5nXTPdGfTu";
String key = "kK4QzkxwN2V8iuyocg6VFj";
String currency = "USD";

//Products
String prodOne = "ESP32";
float prodOneAmount = 0.8;

String prodTwo = "1.4 TFT";
float prodTwoAmount = 1.2;

String prodThree = "Keypad";
float prodThreeAmount = 0.5;

////////////////////////////////////////////////////////
////Note: See lines 75, 97, to adjust to keypad size////
////////////////////////////////////////////////////////

//////////////VARIABLES///////////////////

String dataId = "";
bool paid = false;
bool shouldSaveConfig = false;
bool down = false;
const char* spiffcontent = "";
String spiffing; 
String lnurl;
String choice;
String selection;
String payhash;
String key_val;
String cntr = "0";
String inputs;
int keysdec;
int keyssdec;
float temp;  
String fiat;
float satoshis;
String nosats;
float conversion;
String virtkey;
String payreq;
int randomPin;
bool settle = false;
String preparedURL;
int amount = 0;

#include "MyFont.h"

#define BIGFONT &FreeMonoBold24pt7b
#define MIDBIGFONT &FreeMonoBold18pt7b
#define MIDFONT &FreeMonoBold12pt7b
#define SMALLFONT &FreeMonoBold9pt7b
#define TINYFONT &TomThumb

TFT_eSPI tft = TFT_eSPI();
SHA256 h;

//////////////KEYPAD///////////////////

const byte rows = 4; //four rows
const byte cols = 3; //three columns
char keys[rows][cols] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[rows] = {12, 14, 27, 26};
byte colPins[cols] = {25, 33, 32};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );
int checker = 0;
char maxdig[20];

//////////////MAIN///////////////////

void setup(void) {
  Serial.begin(115200);
  h.begin();
  pinMode(3, OUTPUT); 
  pinMode(22, OUTPUT); 
  pinMode(15, OUTPUT); 
  pinMode(2, OUTPUT); 
  pinMode(4, OUTPUT); 
}

void loop() {
  Serial.println("lnpoo");
  Serial.println("lnpoo");
  wakeUpScreen();
  unsigned long check = millis();
  bool cntr = false;
  selectProduct();
  while (cntr == false){
   char key = keypad.getKey();
   if (key != NO_KEY){
     virtkey = String(key);
       if (virtkey == "1"){
          cntr = true;
          selection = virtkey;
          amount = prodOneAmount * 100;
       }
       else if (virtkey == "2"){
          cntr = true;
          selection = virtkey;
          amount = prodTwoAmount * 100;
       }
       else if (virtkey == "3"){
          cntr = true;
          selection = virtkey;
          amount = prodThreeAmount * 100;
       }
      else if (virtkey == "*"){
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 0);
        tft.setTextColor(TFT_WHITE);
        key_val = "";
        inputs = "";  
        nosats = "";
        virtkey = "";
        cntr = true;
      }  

     }
    if (millis()-check>30000){   
     tft.setFreeFont(SMALLFONT);
     tft.fillScreen(TFT_BLACK);
     tft.setCursor(0, 55);
     tft.setTextColor(TFT_RED, TFT_BLACK);
     tft.print("Sleeping...");
     delay(3000);
     gotoSleep();
    }
  }
  makeLNURL();
  qrShowCode();
  inputs = "";
  cntr = false;
  int pinAttempts = 0;
  while (cntr == false){
    
   char key = keypad.getKey();
   if (key != NO_KEY){
     virtkey = String(key);
       if (virtkey == "*"){
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 0);
        tft.setTextColor(TFT_WHITE);
        key_val = "";
        inputs = "";  
        nosats = "";
        virtkey = "";
        cntr = true;
      }
      showPin();
    }
    if(inputs.length() == 4 && inputs.toInt() == randomPin){
      if(selection == "1"){
        digitalWrite(15, HIGH);
        delay(2500);
        digitalWrite(15, LOW);
        cntr = true;
      }
      if(selection == "2"){
        digitalWrite(2, HIGH);
        delay(2500);
        digitalWrite(2, LOW);
        cntr = true;
      }
      if(selection == "3"){
        digitalWrite(4, HIGH);
        delay(2500);
        digitalWrite(4, LOW);
        cntr = true;
      }
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 0);
        tft.setTextColor(TFT_WHITE);
        key_val = "";
        inputs = "";  
        nosats = "";
        virtkey = "";
        cntr = true;
    }
    else if (inputs.length() == 4 && inputs.toInt() != randomPin){
        tft.setFreeFont(MIDFONT);
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 55);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.print(" Wrong Pin");
        key_val = "";
        inputs = "";  
        nosats = "";
        virtkey = "";
        pinAttempts ++;
        if (pinAttempts > 2){
          tft.setFreeFont(SMALLFONT);
          tft.fillScreen(TFT_BLACK);
          tft.setCursor(0, 55);
          tft.setTextColor(TFT_RED, TFT_BLACK);
          tft.println("   Too many");
          tft.print("   attempts");
          cntr = true;
          delay(3000);
        }
        delay(2000);
        showPin();
    }
  }
}

///////////DISPLAY///////////////

void wakeUpScreen(){
  digitalWrite(22, HIGH);
  delay(500);
  digitalWrite(3, HIGH);
  tft.begin();
  tft.setRotation(1);
  logo();
  delay(2000);
}

void qrShowCode(){
  tft.fillScreen(TFT_WHITE);
  lnurl.toUpperCase();
  const char* lnurlChar = lnurl.c_str();
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(20)];
  qrcode_initText(&qrcode, qrcodeData, 6, 0, lnurlChar);
    for (uint8_t y = 0; y < qrcode.size; y++) {

        // Each horizontal module
        for (uint8_t x = 0; x < qrcode.size; x++) {
            if(qrcode_getModule(&qrcode, x, y)){       
                tft.fillRect(40+2*x, 10+2*y, 2, 2, TFT_BLACK);
            }
            else{
                tft.fillRect(40+2*x, 10+2*y, 2, 2, TFT_WHITE);
            }
        }
    }
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setFreeFont(TINYFONT);
  tft.setCursor(20, 110);
  tft.println("PAY AND ENTER PIN FROM RECEIPT ");
}

void selectProduct()
{
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setFreeFont(SMALLFONT);
  tft.setCursor(0, 10);
  tft.print("1: ");
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println(prodOne);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.print("   " + currency);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println(String(prodOneAmount));
  tft.setCursor(0, 55);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.print("2: ");
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println(prodTwo);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.print("   " + currency);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println(String(prodTwoAmount));
  tft.setCursor(0, 100);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.print("3: ");
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println(prodThree);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.print("   " + currency);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println(String(prodThreeAmount));
}

void showPin(){
  inputs += virtkey;
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setFreeFont(SMALLFONT);
  tft.setCursor(0, 40);
  tft.println(" PROOF PIN");
  tft.setCursor(22, 80);
  tft.setTextColor(TFT_RED, TFT_BLACK); 
  tft.setFreeFont(BIGFONT);
  tft.println(inputs);
  delay(100);
  virtkey = "";
}

void logo(){
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setFreeFont(MIDFONT);
  tft.setCursor(20,60);
  tft.print("LNURLVEND");

  tft.setTextColor(TFT_PURPLE, TFT_BLACK);
  tft.setFreeFont(TINYFONT);
  tft.setCursor(20,70);
  tft.print("Powered by LNbits");
}

void gotoSleep(){ 
  touchAttachInterrupt(T5, wakeUpScreen, 20);
  esp_sleep_enable_touchpad_wakeup();
  esp_deep_sleep_start();
}
void to_upper(char * arr){
  for (size_t i = 0; i < strlen(arr); i++)
  {
    if(arr[i] >= 'a' && arr[i] <= 'z'){
      arr[i] = arr[i] - 'a' + 'A';
    }
  }
}

//////////LNURL AND CRYPTO///////////////

void makeLNURL()
{
  randomPin = random(1000, 9999);
  byte nonce[8];
  for (int i = 0; i < 8; i++)
  {
    nonce[i] = random(256);
  }
  byte payload[51]; // 51 bytes is max one can get with xor-encryption
  size_t payload_len = xor_encrypt(payload, sizeof(payload), (uint8_t *)key.c_str(), key.length(), nonce, sizeof(nonce), randomPin, amount);
  preparedURL = baseURL + "?p=";
  preparedURL += toBase64(payload, payload_len, BASE64_URLSAFE | BASE64_NOPADDING);
  Serial.println(preparedURL);
  char Buf[200];
  preparedURL.toCharArray(Buf, 200);
  char *url = Buf;
  byte *data = (byte *)calloc(strlen(url) * 2, sizeof(byte));
  size_t len = 0;
  int res = convert_bits(data, &len, 5, (byte *)url, strlen(url), 8, 1);
  char *charLnurl = (char *)calloc(strlen(url) * 2, sizeof(byte));
  bech32_encode(charLnurl, "lnurl", data, len);
  to_upper(charLnurl);
  lnurl = charLnurl;
  Serial.println(lnurl);
}

/*
 * Fills output with nonce, xored payload, and HMAC.
 * XOR is secure for data smaller than the key size (it's basically one-time-pad). For larger data better to use AES.
 * Maximum length of the output in XOR mode is 1+1+nonce_len+1+32+8 = nonce_len+43 = 51 for 8-byte nonce.
 * Payload contains pin, currency byte and amount. Pin and amount are encoded as compact int (varint).
 * Returns number of bytes written to the output, 0 if error occured.
 */
int xor_encrypt(uint8_t * output, size_t outlen, uint8_t * key, size_t keylen, uint8_t * nonce, size_t nonce_len, uint64_t pin, uint64_t amount_in_cents){
  // check we have space for all the data:
  // <variant_byte><len|nonce><len|payload:{pin}{amount}><hmac>
  if(outlen < 2 + nonce_len + 1 + lenVarInt(pin) + 1 + lenVarInt(amount_in_cents) + 8){
    return 0;
  }
  int cur = 0;
  output[cur] = 1; // variant: XOR encryption
  cur++;
  // nonce_len | nonce
  output[cur] = nonce_len;
  cur++;
  memcpy(output+cur, nonce, nonce_len);
  cur += nonce_len;
  // payload, unxored first - <pin><currency byte><amount>
  int payload_len = lenVarInt(pin) + 1 + lenVarInt(amount_in_cents);
  output[cur] = (uint8_t)payload_len;
  cur++;
  uint8_t * payload = output+cur; // pointer to the start of the payload
  cur += writeVarInt(pin, output+cur, outlen-cur); // pin code
  cur += writeVarInt(amount_in_cents, output+cur, outlen-cur); // amount
  cur++;
  // xor it with round key
  uint8_t hmacresult[32];
  SHA256 h;
  h.beginHMAC(key, keylen);
  h.write((uint8_t *)"Round secret:", 13);
  h.write(nonce, nonce_len);
  h.endHMAC(hmacresult);
  for(int i=0; i < payload_len; i++){
    payload[i] = payload[i] ^ hmacresult[i];
  }
  // add hmac to authenticate
  h.beginHMAC(key, keylen);
  h.write((uint8_t *)"Data:", 5);
  h.write(output, cur);
  h.endHMAC(hmacresult);
  memcpy(output+cur, hmacresult, 8);
  cur += 8;
  // return number of bytes written to the output
  return cur;
}
