#include <Arduino.h>
#include <WiFi.h>

#include "SinricPro.h"
#include "SinricProSwitch.h"

#define WIFI_SSID         "5Ghz"  // Change WIFI_SSID to your WiFi Name.
#define WIFI_PASS         "chucknorris45"  // Change WIFI_PASS to your WiFi password.
#define APP_KEY           "849de8ef-55fe-4aaa-9df3-a671eb2927a5"  // Paste App Key from above 
#define APP_SECRET        "ab60ea94-9aab-4711-89ce-f77f0126efe8-c5fe95c8-6293-478b-867a-4254444258aa"  // Paste App Secret from above 
#define SWITCH_ID_1       "661833ca7c9e6c6fe868fa46"  // Paste Device Id from  above

// define led according to pin diagram
int led = D10;

bool onPowerState1(const String &deviceId, bool &state) {
  
  Serial.printf("Device 1 turned %s\n", state?"on":"off");
  digitalWrite(led, state ? HIGH:LOW);
 
 return true; // request handled properly
}

void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.setSleep(false); 
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

// setup function for SinricPro
void setupSinricPro() {
  // add devices and callbacks to SinricPro
  pinMode(led, OUTPUT); 

  SinricProSwitch& mySwitch1 = SinricPro[SWITCH_ID_1];
  mySwitch1.onPowerState(onPowerState1);
    
  // setup SinricPro
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); }); 
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
   
  SinricPro.begin(APP_KEY, APP_SECRET);
}

void setup() {
  // initialize digital pin led as an output
  Serial.begin(115200); Serial.printf("\r\n\r\n");
  setupWiFi();
  setupSinricPro();
}

void loop() {
  SinricPro.handle();
}