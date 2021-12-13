# LNURLVend
## Offline bitcoin vending machine

LNURLVend is the next logic step after <a href="https://github.com/arcbtc/LNURLPoS">LNURLPoS</a>, where you cango to read more about how LNURLPoS works.

### Demo video

### Tutorial video

### Hardware layout
![vending](https://user-images.githubusercontent.com/33088785/145814575-58988069-48b9-4e1d-8aa2-85be552be4c8.png)

### Arduino software install

* Download/install latest <a href="https://www.arduino.cc/en/software">Arduino IDE</a>
* Install ESP32 boards, using <a href="https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html#installing-using-boards-manager">boards manager</a>
* Copy <a href="https://github.com/arcbtc/LNURLPoS/tree/main/LNURLPoS/libraries">these libraries</a> into your Arduino IDE library folder
* Plug in T-Display, from *Tools>Board>ESP32 Boards* select **ESP32 DEV MODULE**

> *Note: If using MacOS, you will need the CP210x USB to UART Bridge VCP Drivers available here https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers*

> *Note: You may need to roll your ESP32 boards back to an earlier version in the Arduino IDE, by using tools>boards>boards manager, searching for esp. I use v1.0.5(rc6), and have also used v1.0.4 which worked.*
### LNbits extension

To make things easy (usually a few clicks on things like Raspiblitz), there is an <a href="https://github.com/lnbits/lnbits/tree/master/lnbits/extensions/lnurlpos">LNbits extension</a>.
If you want to make your own stand-alone server software that would be fairly easy to do, by replicating the lnurl.py file in the extennsion.

### Future updates 
At the beginning of this article I said "LNURLPoS (currently) only uses LNURL-Pay". The next stage will be for the PoS to also create LNURL-Withdraws, which are essentially faucets. This means merchants can offer refunds, and also sell bitcoin over the counter, which creates an extremely powerful tool for local economies on-ramping and off-ramping from their local fiat currency.

At Adopting Bitcoin in San Salvador I will distribute 40 kits over x2 workshops, so hopefully some locals will start producing, selling and teaching others how to make these useful little units.
