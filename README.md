# SmartCarProject

## Video Overview:
The following video gives a demonstration of the project's scope, costs, functionality, challenges, and shortcomings.
https://www.youtube.com/watch?v=QAWRW4KXTsA&t=8s&ab_channel=WhittenOswald

## Materials:
### Hardware

-Microcontroller:
TI-MSP430G2553: https://www.ti.com/product/MSP430G2553

-HCSR-04 Ultrasonic Sensors:
https://www.amazon.com/HC-SR04-Ultrasonic-Distance-Arduino-MEGA2560/dp/B07R7RMXTY/ref=sr_1_7?crid=19ACPCGL8DX7L&keywords=hcsr04&qid=1659626075&sprefix=hcsr%2Caps%2C94&sr=8-7

-Wi-Fi module:
https://www.amazon.com/DIYmall-ESP8266-ESP-01S-Serial-Transceiver/dp/B00O34AGSU/ref=sr_1_4?crid=1BPEONGT01K7L&keywords=wifi+module&qid=1659626039&sprefix=wifi+module%2Caps%2C123&sr=8-4


### Software / IDEs
-Code Composer Studio: 
https://software-dl.ti.com/ccs/esd/documents/ccs_downloads.html

-Microsoft Visual C++:
https://docs.microsoft.com/en-us/cpp/?view=msvc-170

-ThingSpeak channel:
https://thingspeak.com/

-Arduino IDE:
https://www.arduino.cc/en/software

-Textbook reference (PDF Drive):
Programmable Microcontrollers with Applications (Unsalan, Gurhan) https://www.pdfdrive.com/deniz-programmable-microcontrollers-with-applications-msp430-launchpad-with-ccs-and-grace-e196719615.html


## Making it work:
If using the Wi-Fi module: Be sure to insert your ThingSpeak channel #, Write API key, and WiFi SSID / password into ESP8266.ino before flashing the module.
Then you can consult this resource for the corresponding pinout diagram: https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/

If using a hardwired serial connection: Be sure to consult your computer's Device Manager 'Ports' section to determine the correct Port name/number. The default for this code is COM6. SmartCarGUI.cs will need to be updated to reflect these changes. Update the C# code also (if using ThingSpeak) with the relevant URL and field # for your channel.

The final challenge will be to wire up the corresponding pins from the MSP430G2553 (see Textbook Reference) to the sensors using a Breadboard or other circuit board.








