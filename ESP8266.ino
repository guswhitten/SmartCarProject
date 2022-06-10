/*
This Arduino file is flashed to an ESP8266 wi-fi module. 
The module can then be used to send data serially to a ThingSpeak channel.
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ThingSpeak.h>
#include <SPI.h>
#include <Wire.h>

const char* ssid = "typeYourSSIDhere";  //add your wifi SSID in the quotes
const char* pass = "typerYourPASShere"; //add your wifi password in the quotes
unsigned long myChannelNumber = 0000000; // replace 0000000 with your channel number
const char * myWriteAPIKey = ""; // replace MyAPIKEY with your thingspeak write API key
const unsigned int MAX_MESSAGE_LENGTH = 8;

WiFiClient client;
ThingSpeakClass ts;

void init_WIFI()
{
  while (WiFi.status() != WL_CONNECTED) 
  {
    WiFi.begin(ssid, pass);
    delay(5000);
  }
}

void setup(){
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass);
    delay(5000);
  }
  //Serial.println("\nConnected.");
  Serial.begin(19200);
  WiFi.mode(WIFI_STA);
  ts.begin(client);
}

void loop(void){
  if (WiFi.status() != WL_CONNECTED) 
  {
    init_WIFI();
  }
  int newByte = 0;
  char c;
  String first = "";
  String second = "";
  String third = "";
  bool receivedEOM = false;
  int count = 1;
  while (Serial.available() == 0) {
    delay(100);
  }
  while (!receivedEOM) {
    if (Serial.available() > 0) {
      newByte = Serial.read();
      c = (char) newByte;
      if (c == ';') {
        receivedEOM = true;
      }
      else if (count < 4) {
        first += c;
      }
      else if (count > 3 && count < 6) {
        second += c;
      }
      else {
        third += c;
      }
      count++;
    }
    delay(100);
  }
  ts.setField(1, first);
  ts.setField(2, second);
  ts.setField(3, third);
  ts.writeFields(myChannelNumber, myWriteAPIKey);
  delay(15500);
}
