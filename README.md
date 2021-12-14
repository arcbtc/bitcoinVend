<img style="width:500px" src="https://user-images.githubusercontent.com/33088785/145907555-c2d870a7-46aa-419e-a12b-8502c80c0849.png">

# LNURLVend
## Offline bitcoin vending machine

LNURLVend is the next logic step after <a href="https://github.com/arcbtc/LNURLPoS">LNURLPoS</a> <-- go that repo to learn more about the concepts behind LNURLVend.

### Demo video

https://twitter.com/arcbtc/status/1470541850566090757

### Tutorial video

Coming very soon...

## Hardware

![145817489-0223e99f-537b-4852-a8cb-26ceba8d4a5a](https://user-images.githubusercontent.com/33088785/145819968-96b2c263-cbf7-4b20-9237-aabe1fec5373.png)

* Vending Machine. I use this excellent <a href="https://www.alibaba.com/product-detail/QR-code-payment-Mini-hotel-vending_1600208572998.html">vending machine</a> 🤩
* ESP32 DEV MODULE
* 12V to 5V Converter USB,3A
* male/male, female/female, male/female jumper wires
* Keypad membrane
* 1.4inch TFT ST7735
* 12v battery (optional)
* Wide breadboard (I used x2 normal breadboards stuck together)
* 4 Channel Relay Module
* Single relay module

**Keypad membrane GPIO map:** 
12-> 32, last row (A,B,C,D) GND

**TFT GPIO map:** 
[VCC - 5V (on the single relay), GND - GND, CS - GPIO5, Reset - GPIO16, AO (DC) - GPI17, SDA (MOSI) - GPIO23, SCK - GPIO18, LED - 3.3V (on the single relay)]

![vending](https://user-images.githubusercontent.com/33088785/145814575-58988069-48b9-4e1d-8aa2-85be552be4c8.png)

## Software

### Arduino software install

* Download/install latest <a href="https://www.arduino.cc/en/software">Arduino IDE</a>
* Install ESP32 boards, using <a href="https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html#installing-using-boards-manager">boards manager</a>
* Copy <a href="https://github.com/arcbtc/LNURLPoS/tree/main/LNURLVend/libraries">these libraries</a> into your Arduino IDE library folder
* Plug in T-Display, from *Tools>Board>ESP32 Boards* select **ESP32 DEV MODULE**

> *Note: You may need to roll your ESP32 boards back to an earlier version in the Arduino IDE, by using tools>boards>boards manager, searching for esp. I use v1.0.5(rc6), and have also used v1.0.4 which worked.*
### LNbits extension

To make things easy (usually a few clicks on things like Raspiblitz), there is an <a href="https://github.com/lnbits/lnbits/tree/master/lnbits/extensions/lnurlpos">LNbits extension</a>.
If you want to make your own stand-alone server software that would be fairly easy to do, by replicating the lnurl.py file in the extennsion.

### Future updates 
Looking forward to seeing this same project being used in a range of vending machines, all shapes and sizes.
