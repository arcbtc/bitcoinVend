
#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>
using WebServerClass = WebServer;
fs::SPIFFSFS &FlashFS = SPIFFS;
#define FORMAT_ON_FAIL true

#include <Keypad.h>
#include <AutoConnect.h>
#include <SPI.h>
#include <Wire.h>
#include <TFT_eSPI.h>
#include <Hash.h>
#include <ArduinoJson.h>
#include "qrcoded.h"
#include "Bitcoin.h"
#include "esp_adc_cal.h"

#define PARAM_FILE "/elements.json"
#define KEY_FILE "/thekey.txt"

//Variables // 15,2,3 pins

/////////////////////////////////
///////////CHANGE////////////////
/////////////////////////////////
         
bool format = false; // true for formatting SPIFFS, use once, then make false and reflash
String apPassword = "ToTheMoon1"; //default WiFi AP password
String content = "<h1>bitcoinVend</br>Free Open-Source bitcoin Vending Machine</h1>";

/////////////////////////////////
/////////////////////////////////


String inputs;
String thePin;
String nosats;
String cntr = "0";
String lnurl;
String currency;
String lncurrency;
String key;
String preparedURL;
String baseURL;
String masterKey;
String lnbitsServer;
String invoice;
String baseURLvend;
String secretvend;
String currencyvend;
String dataIn = "0";
String amountToShow = "0.00";
String noSats = "0";
String qrData;
String dataId;
String addressNo;
String pinToShow;
String bitcoinVendProdNames[] = {"", "", "", "", "", "", "", "", ""};
String bitcoinVendProdAmounts[] = {"", "", "", "", "", "", "", "", ""};
String bitcoinVendProdPins[] = {"", "", "", "", "", "", "", "", ""};
String selection;
String virtkey;
int menuItemNo = 0;
int randomPin;
int calNum = 1;
int sumFlag = 0;
int converted = 0;
int bitcoinVendTime = 0;
int amount = 0;
String key_val;
bool onchainCheck = false;
bool lnCheck = false;
bool lnurlCheck = false;
bool unConfirmed = true;
bool selected = false;
bool lnurlCheckvend = false;


//Custom access point pages
static const char PAGE_ELEMENTS[] PROGMEM = R"(
{
  "uri": "/config",
  "title": "bitcoinVend Options",
  "menu": true,
  "element": [
    {
      "name": "text",
      "type": "ACText",
      "value": "bitcoinVend options",
      "style": "font-family:Arial;font-size:16px;font-weight:400;color:#191970;margin-botom:15px;"
    },
    {
      "name": "password",
      "type": "ACInput",
      "label": "Password for vend AP WiFi",
      "value": "ToTheMoon1"
    },
    {
      "name": "bitcoinvendpos",
      "type": "ACInput",
      "label": "LNURLPoS string from LNbits extension"
    },
    {
      "name": "bitcoinvendmotortime",
      "type": "ACInput",
      "label": "Motor time millisecs",
      "vale": "2500"
    },
    {
      "name": "bitcoinvendprodone",
      "type": "ACInput",
      "label": "Product One",
      "value": "sweets,0.10,2"
    },
     {
      "name": "bitcoinvendprodtwo",
      "type": "ACInput",
      "label": "Product Two",
      "value": ""
    },
     {
      "name": "bitcoinvendprodthree",
      "type": "ACInput",
      "label": "Product Three",
      "value": ""
    },
     {
      "name": "bitcoinvendprodfour",
      "type": "ACInput",
      "label": "Product Four",
      "value": ""
    },
     {
      "name": "bitcoinvendprodfive",
      "type": "ACInput",
      "label": "Product Five",
      "value": ""
    },
     {
      "name": "bitcoinvendprodsix",
      "type": "ACInput",
      "label": "Product Six",
      "value": ""
    },
     {
      "name": "bitcoinvendprodseven",
      "type": "ACInput",
      "label": "Product Seven",
      "value": ""
    },
     {
      "name": "bitcoinvendprodeight",
      "type": "ACInput",
      "label": "Product Eight",
      "value": ""
    },
     {
      "name": "bitcoinvendprodnine",
      "type": "ACInput",
      "label": "Product Nine",
      "value": ""
    },
    {
      "name": "load",
      "type": "ACSubmit",
      "value": "Load",
      "uri": "/config"
    },
    {
      "name": "save",
      "type": "ACSubmit",
      "value": "Save",
      "uri": "/save"
    },
    {
      "name": "adjust_width",
      "type": "ACElement",
      "value": "<script type='text/javascript'>window.onload=function(){var t=document.querySelectorAll('input[]');for(i=0;i<t.length;i++){var e=t[i].getAttribute('placeholder');e&&t[i].setAttribute('size',e.length*.8)}};</script>"
    }
  ]
 }
)";

static const char PAGE_SAVE[] PROGMEM = R"(
{
  "uri": "/save",
  "title": "Elements",
  "menu": false,
  "element": [
    {
      "name": "caption",
      "type": "ACText",
      "format": "Elements have been saved to %s",
      "style": "font-family:Arial;font-size:18px;font-weight:400;color:#191970"
    },
    {
      "name": "validated",
      "type": "ACText",
      "style": "color:red"
    },
    {
      "name": "echo",
      "type": "ACText",
      "style": "font-family:monospace;font-size:small;white-space:pre;"
    },
    {
      "name": "ok",
      "type": "ACSubmit",
      "value": "OK",
      "uri": "/vendconfig"
    }
  ]
}
)";

TFT_eSPI tft = TFT_eSPI();

SHA256 h;
WebServerClass server;
AutoConnect portal(server);
AutoConnectConfig config;
AutoConnectAux elementsAux;
AutoConnectAux saveAux;

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

void setup()
{
  Serial.begin(115200);
  
  h.begin();
  pinMode(3, OUTPUT); 
  pinMode(22, OUTPUT);

  // load buttons
  
  FlashFS.begin(FORMAT_ON_FAIL);
  SPIFFS.begin(true);
  if(format == true){
    SPIFFS.format(); 
  }
  // get the saved details and store in global variables
  File paramFile = FlashFS.open(PARAM_FILE, "r");
  if (paramFile)
  {
    StaticJsonDocument<2500> doc;
    DeserializationError error = deserializeJson(doc, paramFile.readString());

    const JsonObject lnurlVPass = doc[0];
    const char *apPasswordChar = lnurlVPass["value"];
    const char *apNameChar = lnurlVPass["name"];
    if (String(apPasswordChar) != "" && String(apNameChar) == "password")
    {
      apPassword = apPasswordChar;
    }

    const JsonObject lnurlVRoot = doc[1];
    const char *bitcoinvendChar = lnurlVRoot["value"];
    String bitcoinvend = bitcoinvendChar;
    baseURLvend = getValue(bitcoinvend, ',', 0);
    secretvend = getValue(bitcoinvend, ',', 1);
    currencyvend = getValue(bitcoinvend, ',', 2);

    const JsonObject lnurlVTime = doc[2];
    const char *bitcoinvendCharTime = lnurlVTime["value"];
    bitcoinVendTime = String(bitcoinvendCharTime).toInt();
    
    if(doc[3] != ""){
      const JsonObject lnurlVOne = doc[3];
      const char *bitcoinVendProdOneChar = lnurlVOne["value"];
      String bitcoinVendProdOneStr = bitcoinVendProdOneChar;
      bitcoinVendProdNames[0] = getValue(bitcoinVendProdOneStr, ',', 0);
      bitcoinVendProdAmounts[0] = getValue(bitcoinVendProdOneStr, ',', 1);
      bitcoinVendProdPins[0] = getValue(bitcoinVendProdOneStr, ',', 2);
      pinMode(bitcoinVendProdPins[0].toInt(), OUTPUT); 
    }

    if(doc[4] != ""){
      const JsonObject lnurlVTwo = doc[4];
      const char *bitcoinVendProdTwoChar = lnurlVTwo["value"];
      String bitcoinVendProdTwoStr = bitcoinVendProdTwoChar;
      bitcoinVendProdNames[1] = getValue(bitcoinVendProdTwoStr, ',', 0);
      bitcoinVendProdAmounts[1] = getValue(bitcoinVendProdTwoStr, ',', 1);
      bitcoinVendProdPins[1] = getValue(bitcoinVendProdTwoStr, ',', 2);
      pinMode(bitcoinVendProdPins[1].toInt(), OUTPUT); 
    }

    if(doc[5] != ""){
      const JsonObject lnurlVThree = doc[5];
      const char *bitcoinVendProdThreeChar = lnurlVThree["value"];
      String bitcoinVendProdThreeStr = bitcoinVendProdThreeChar;
      bitcoinVendProdNames[2] = getValue(bitcoinVendProdThreeStr, ',', 0);
      bitcoinVendProdAmounts[2] = getValue(bitcoinVendProdThreeStr, ',', 1);
      bitcoinVendProdPins[2] = getValue(bitcoinVendProdThreeStr, ',', 2);
      pinMode(bitcoinVendProdPins[2].toInt(), OUTPUT); 
    }

    if(doc[6] != ""){
      const JsonObject lnurlVFour = doc[6];
      const char *bitcoinVendProdFourChar = lnurlVFour["value"];
      String bitcoinVendProdFourStr = bitcoinVendProdFourChar;
      bitcoinVendProdNames[3] = getValue(bitcoinVendProdFourStr, ',', 0);
      bitcoinVendProdAmounts[3] = getValue(bitcoinVendProdFourStr, ',', 1);
      bitcoinVendProdPins[3] = getValue(bitcoinVendProdFourStr, ',', 2);
      pinMode(bitcoinVendProdPins[3].toInt(), OUTPUT); 
    }

    if(doc[7] != ""){
      const JsonObject lnurlVFive = doc[7];
      const char *bitcoinVendProdFiveChar = lnurlVFive["value"];
      String bitcoinVendProdFiveStr = bitcoinVendProdFiveChar;
      bitcoinVendProdNames[4] = getValue(bitcoinVendProdFiveStr, ',', 0);
      bitcoinVendProdAmounts[4] = getValue(bitcoinVendProdFiveStr, ',', 1);
      bitcoinVendProdPins[4] = getValue(bitcoinVendProdFiveStr, ',', 2);
      pinMode(bitcoinVendProdPins[4].toInt(), OUTPUT); 
    }

    if(doc[8] != ""){
      const JsonObject lnurlVSix = doc[8];
      const char *bitcoinVendProdSixChar = lnurlVSix["value"];
      String bitcoinVendProdSixStr = bitcoinVendProdSixChar;
      bitcoinVendProdNames[5] = getValue(bitcoinVendProdSixStr, ',', 0);
      bitcoinVendProdAmounts[5] = getValue(bitcoinVendProdSixStr, ',', 1);
      bitcoinVendProdPins[5] = getValue(bitcoinVendProdSixStr, ',', 2);
      pinMode(bitcoinVendProdPins[5].toInt(), OUTPUT); 
    }

    if(doc[9] != ""){
      const JsonObject lnurlVSeven = doc[9];
      const char *bitcoinVendProdSevenChar = lnurlVSeven["value"];
      String bitcoinVendProdSevenStr = bitcoinVendProdSevenChar;
      bitcoinVendProdNames[6] = getValue(bitcoinVendProdSevenStr, ',', 0);
      bitcoinVendProdAmounts[6] = getValue(bitcoinVendProdSevenStr, ',', 1);
      bitcoinVendProdPins[6] = getValue(bitcoinVendProdSevenStr, ',', 2);
      pinMode(bitcoinVendProdPins[6].toInt(), OUTPUT); 
    }

    if(doc[10] != ""){
      const JsonObject lnurlVEight = doc[10];
      const char *bitcoinVendProdEightChar = lnurlVEight["value"];
      String bitcoinVendProdEightStr = bitcoinVendProdEightChar;
      bitcoinVendProdNames[7] = getValue(bitcoinVendProdEightStr, ',', 0);
      bitcoinVendProdAmounts[7] = getValue(bitcoinVendProdEightStr, ',', 1);
      bitcoinVendProdPins[7] = getValue(bitcoinVendProdEightStr, ',', 2);
      pinMode(bitcoinVendProdPins[7].toInt(), OUTPUT); 
    }

    if(doc[11] != ""){
      const JsonObject lnurlVNine = doc[11];
      const char *bitcoinVendProdNineChar = lnurlVNine["value"];
      String bitcoinVendProdNineStr = bitcoinVendProdNineChar;
      bitcoinVendProdNames[8] = getValue(bitcoinVendProdNineStr, ',', 0);
      bitcoinVendProdAmounts[8] = getValue(bitcoinVendProdNineStr, ',', 1);
      bitcoinVendProdPins[8] = getValue(bitcoinVendProdNineStr, ',', 2);
      pinMode(bitcoinVendProdPins[8].toInt(), OUTPUT); 
    }
  }

  paramFile.close();

  // general WiFi setting
  config.autoReset = false;
  config.autoReconnect = true;
  config.reconnectInterval = 1; // 30s
  config.beginTimeout = 10000UL;

  // start portal (any key pressed on startup)
  const char key = keypad.getKey();
  if (key != NO_KEY)
  {
    // start access point
    portalLaunch();
    // handle access point traffic
    server.on("/", []() {
      content += AUTOCONNECT_LINK(COG_24);
      server.send(200, "text/html", content);
    });
    elementsAux.load(FPSTR(PAGE_ELEMENTS));
    elementsAux.on([](AutoConnectAux &aux, PageArgument &arg) {
      File param = FlashFS.open(PARAM_FILE, "r");
      if (param)
      {
        aux.loadElement(param, {"password", "bitcoinvendpos", "bitcoinvendmotortime", "bitcoinvendprodone", "bitcoinvendprodtwo", "bitcoinvendprodthree", "bitcoinvendprodfour", "bitcoinvendprodfive", "bitcoinvendprodsix", "bitcoinvendprodseven", "bitcoinvendprodeight", "bitcoinvendprodnine"});
        param.close();
      }

      if (portal.where() == "/config")
      {
        File param = FlashFS.open(PARAM_FILE, "r");
        if (param)
        {
          aux.loadElement(param, {"password", "bitcoinvendpos", "bitcoinvendmotortime", "bitcoinvendprodone", "bitcoinvendprodtwo", "bitcoinvendprodthree", "bitcoinvendprodfour", "bitcoinvendprodfive", "bitcoinvendprodsix", "bitcoinvendprodseven", "bitcoinvendprodeight", "bitcoinvendprodnine"});
          param.close();
        }
      }
      return String();
    });

    saveAux.load(FPSTR(PAGE_SAVE));
    saveAux.on([](AutoConnectAux &aux, PageArgument &arg) {
      aux["caption"].value = PARAM_FILE;
      File param = FlashFS.open(PARAM_FILE, "w");

      if (param)
      {
        // save as a loadable set for parameters.
        elementsAux.saveElement(param, {"password", "bitcoinvendpos", "bitcoinvendmotortime", "bitcoinvendprodone", "bitcoinvendprodtwo", "bitcoinvendprodthree", "bitcoinvendprodfour", "bitcoinvendprodfive", "bitcoinvendprodsix", "bitcoinvendprodseven", "bitcoinvendprodeight", "bitcoinvendprodnine"});
        param.close();

        // read the saved elements again to display.
        param = FlashFS.open(PARAM_FILE, "r");
        aux["echo"].value = param.readString();
        param.close();
      }
      else
      {
        aux["echo"].value = "Filesystem failed to open.";
      }

      return String();
    });

    config.immediateStart = true;
    config.ticker = true;
    config.apid = "bitcoinVend-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    config.psk = apPassword;
    config.menuItems = AC_MENUITEM_CONFIGNEW | AC_MENUITEM_OPENSSIDS | AC_MENUITEM_RESET;
    config.title = "bitcoinVend";

    portal.join({elementsAux, saveAux});
    portal.config(config);
    portal.begin();
    while (true)
    {
      portal.handleClient();
    }
  }
    // connect to configured WiFi
  if (baseURLvend[0] < 1)
  {
    config.autoRise = false;
    portal.join({elementsAux, saveAux});
    portal.config(config);
    portal.begin();
  }
}

void loop() {
   wakeUpScreen();
   unsigned long check = millis();
   bool cntr = false;
   selectProduct();
   inputs = "";
   int timer;
   while (cntr == false){
     char key = keypad.getKey();
     if (key != NO_KEY){
       virtkey = String(key);
       for (int i = 0; i < sizeof(bitcoinVendProdNames); i++){
         if (bitcoinVendProdNames[i] != ""){
           if (virtkey == String(i+1)){
             selection = virtkey;
             amount = bitcoinVendProdAmounts[i].toFloat() * 100;
             makeLNURL();
             qrShowCode();
             inputs = "";
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
                   else{
                     showPin();
                   }
                 
               }
               if(inputs.length() == 4 && inputs.toInt() == randomPin){
                 if(selection == String(i+1)){
                   digitalWrite(bitcoinVendProdPins[i].toInt(), HIGH);
                   delay(bitcoinVendTime);
                   digitalWrite(bitcoinVendProdPins[i].toInt(), LOW);
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
                 wrongPin();
                 key_val = "";
                 inputs = "";  
                 nosats = "";
                 virtkey = "";
                 pinAttempts ++;
                 if (pinAttempts > 2){
                   tooManyAttempts();
                   cntr = true;
                   delay(3000);
                 }
                 delay(2000);
                 showPin();
               }  
             }
           }
         }
       }  
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
    }
    delay(200);
    if (millis()-check>500000){   
      aaannndddSleep();
      delay(3000);
      gotoSleep();
    } 
  }
}

///////////DISPLAY///////////////

void tooManyAttempts(){
  tft.setTextSize(2);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 55);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.println("   Too many");
  tft.print("   attempts");
}

void aaannndddSleep(){
  tft.setTextSize(2);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 55);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.print("Sleeping zzz..");
}

void wrongPin(){
  tft.setTextSize(2);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 55);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.print(" Wrong Pin");
}

void portalLaunch(){
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_PURPLE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(15, 50);
  tft.println("AP LAUNCHED");
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(0, 100);
  tft.setTextSize(1);
  tft.println("WHEN FINISHED RESET");
}

void wakeUpScreen(){
  digitalWrite(22, HIGH);
  delay(500);
  digitalWrite(3, HIGH);
  tft.begin();
  tft.setRotation(1);
}

void qrShowCode(){
  tft.fillScreen(TFT_WHITE);
  qrData.toUpperCase();
  const char* lnurlChar = qrData.c_str();
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
  tft.setTextSize(1);
  tft.setCursor(0, 110);
  tft.println("PAY AND ENTER PIN FROM RECEIPT ");
}

void selectProduct()
{
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 20);
  tft.println("bitcoinVend");
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(10, 50);
  tft.println("Select a product");
}

void showPin(){
  inputs += virtkey;
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(0, 40);
  tft.println(" PROOF PIN");
  tft.setCursor(22, 80);
  tft.setTextColor(TFT_RED, TFT_BLACK); 
  tft.setTextSize(3);
  tft.println(inputs);
  delay(100);
  virtkey = "";
}

void logo()
{
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(0, 30);
  tft.print("bitcoin");
  tft.setTextColor(TFT_PURPLE, TFT_BLACK);
  tft.print("Vend");
  tft.setTextSize(1);
  tft.setCursor(0, 80);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.print("Powered by LNbits");
  delay(2000);
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

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  const int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
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

  size_t payload_len = xor_encrypt(payload, sizeof(payload), (uint8_t *)secretvend.c_str(), secretvend.length(), nonce, sizeof(nonce), randomPin, amount);
  preparedURL = baseURLvend + "?p=";
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
  qrData = charLnurl;
  Serial.println(qrData);
}

int xor_encrypt(uint8_t *output, size_t outlen, uint8_t *key, size_t keylen, uint8_t *nonce, size_t nonce_len, uint64_t pin, uint64_t amount_in_cents)
{
  // check we have space for all the data:
  // <variant_byte><len|nonce><len|payload:{pin}{amount}><hmac>
  if (outlen < 2 + nonce_len + 1 + lenVarInt(pin) + 1 + lenVarInt(amount_in_cents) + 8)
  {
    return 0;
  }
  int cur = 0;
  output[cur] = 1; // variant: XOR encryption
  cur++;
  // nonce_len | nonce
  output[cur] = nonce_len;
  cur++;
  memcpy(output + cur, nonce, nonce_len);
  cur += nonce_len;
  // payload, unxored first - <pin><currency byte><amount>
  int payload_len = lenVarInt(pin) + 1 + lenVarInt(amount_in_cents);
  output[cur] = (uint8_t)payload_len;
  cur++;
  uint8_t *payload = output + cur;                                 // pointer to the start of the payload
  cur += writeVarInt(pin, output + cur, outlen - cur);             // pin code
  cur += writeVarInt(amount_in_cents, output + cur, outlen - cur); // amount
  cur++;
  // xor it with round key
  uint8_t hmacresult[32];
  SHA256 h;
  h.beginHMAC(key, keylen);
  h.write((uint8_t *)"Round secret:", 13);
  h.write(nonce, nonce_len);
  h.endHMAC(hmacresult);
  for (int i = 0; i < payload_len; i++)
  {
    payload[i] = payload[i] ^ hmacresult[i];
  }
  // add hmac to authenticate
  h.beginHMAC(key, keylen);
  h.write((uint8_t *)"Data:", 5);
  h.write(output, cur);
  h.endHMAC(hmacresult);
  memcpy(output + cur, hmacresult, 8);
  cur += 8;
  // return number of bytes written to the output
  return cur;
}
