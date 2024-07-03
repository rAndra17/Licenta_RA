#include <Arduino.h>
#include <WiFi.h>
#include "SinricPro.h"
#include "SinricProLight.h"
#include <NimBLEDevice.h>

#define WIFI_SSID       "Nume"    // numele retelei 
#define WIFI_PASS       "Parola"  // parola de acces
#define APP_KEY         "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"  // App Key 
#define APP_SECRET      "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx-xxx..."  // App Secret  
#define LIGHT_ID        "Id_dizpozitiv"  // Device Id 

static NimBLEAddress serverAddress ("74:d2:85:5e:40:34"); //adresa MAC a ghirlandei
static NimBLEUUID serviceUUID("1000");
static NimBLEUUID characteristicUUID("1001");

NimBLEClient* pClient = nullptr;
NimBLERemoteService* pService = nullptr;
NimBLERemoteCharacteristic* pCharacteristic = nullptr;

uint8_t rgb_color_data[] = { 0x3c, 0x02, 0x00, 0x00, 0x00 };
uint8_t on_off_data[] = { 0x3c, 0x01, 0x00 }; //on 3c0101, off 3c0100

bool onPowerState(const String &deviceId, bool &state) {
  
  Serial.printf("Device turned %s\n", state?"on":"off");
  on_off_data[2] = state ? 0x01 : 0x00;
  //disconnect from wifi and turn off the wifi radio
  disconnectWiFi();

  //initialize bt
  NimBLEDevice::init("");
  NimBLEDevice::setPower(ESP_PWR_LVL_P9);
  // Connect to the bt server
  pClient = NimBLEDevice::createClient();
  if (pClient->connect(serverAddress)) {
    Serial.println("Connected to server");
    // Get the service and characteristic
    pService = pClient->getService(serviceUUID);
    if (pService){
      pCharacteristic = pService->getCharacteristic(characteristicUUID);
      if ( pCharacteristic ) {
        if (pCharacteristic->writeValue(on_off_data,sizeof(on_off_data), 0)) {
          Serial.println("Write successful");
          delay(500);
        } else {
          Serial.println("Write failed");
        }
      }
    }
    
  } else {
    Serial.println("Failed to connect to server");
  }

  //disconect bt
  disconnectBLE();
  reconnectWiFi();
  return true; // request handled properly
}

bool onColor(const String &deviceId, byte &r, byte &g, byte &b) {
  rgb_color_data[2] = r;
  rgb_color_data[3] = g;
  rgb_color_data[4] = b;
  Serial.printf("Device color changed to %d, %d, %d (RGB)\r\n", rgb_color_data[2], rgb_color_data[3], rgb_color_data[4]);

  //disconnect from wifi and turn off the wifi radio
  disconnectWiFi();
  //initialize bt
  NimBLEDevice::init("");
  NimBLEDevice::setPower(ESP_PWR_LVL_P9);
  // Connect to the bt server
  pClient = NimBLEDevice::createClient();
  if (pClient->connect(serverAddress)) {
    Serial.println("Connected to server");
    // Get the service and characteristic
    pService = pClient->getService(serviceUUID);
    if (pService){
      pCharacteristic = pService->getCharacteristic(characteristicUUID);
      if ( pCharacteristic ) {
        if (pCharacteristic->writeValue(rgb_color_data,sizeof(rgb_color_data), 0)) {
          Serial.println("Write successful");
          delay(500);
        } else {
          Serial.println("Write failed");
        }
      }
    }
    
  } else {
    Serial.println("Failed to connect to server");
  }

  //disconect bt
  disconnectBLE();
  reconnectWiFi();
  return true;
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

void reconnectWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(500);
  }
  
  Serial.printf("");
  Serial.printf("\n WiFi reconnected\n");
}

void disconnectWiFi() {
    WiFi.disconnect(true);
    delay(500);
    Serial.printf("WiFi disconnected");
}

void disconnectBLE() {
  pClient->disconnect();
  delay(500);

  NimBLEDevice::deleteClient(pClient); // Delete the client object
  NimBLEDevice::deinit(); // Deinitialize NimBLE
  pClient = nullptr; // Reset the pointer
  pService = nullptr;
  pCharacteristic = nullptr;
  Serial.println("BLE disconnected and resources freed");
}

// setup function for SinricPro
void setupSinricPro() {

 SinricProLight &myLight = SinricPro[LIGHT_ID];

  // set callback function to device
  myLight.onPowerState(onPowerState);
  myLight.onColor(onColor);
  
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
  if (WiFi.status() == WL_CONNECTED) {
        SinricPro.handle();
    } else {
        Serial.printf("Failed to reconnect to WiFi");
    }
}