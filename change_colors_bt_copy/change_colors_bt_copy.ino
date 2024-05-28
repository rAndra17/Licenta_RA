#include "BLEDevice.h"

const char* serverUUID = "74:d2:85:5e:40:34";
static BLEUUID serviceUUID("1000");
static BLEUUID characteristicUUID("1001");
bool connected = false;

BLEClient* pMyClient = BLEDevice::createClient();
BLEAddress address(serverUUID);
BLERemoteService* pMyRemoteService;
BLERemoteCharacteristic* pMyRemoteCharacteristic;

uint8_t rgb_color_data[] = { 0x3c, 0x02, 0x00, 0x00, 0x00 };
uint8_t on_off_data[] = { 0x3c, 0x01, 0x00 }; //on 3c0101, off 3c0100

void setup() {
  BLEDevice::init("");
}

//function to write RGB values and delay
void writeRGB (uint8_t r, uint8_t g, uint8_t b) {
  rgb_color_data[2] = r;
  rgb_color_data[3] = g;
  rgb_color_data[4] = b;
  pMyRemoteCharacteristic->writeValue(rgb_color_data, sizeof(rgb_color_data));
  delay(500);
};

void loop() {
 
  if ( !connected ) {
    if ( pMyClient->connect(address) ) {
      connected = true;
      pMyRemoteService = pMyClient->getService(serviceUUID);
      pMyRemoteCharacteristic = pMyRemoteService->getCharacteristic(characteristicUUID);
    }
  }

  if ( connected ) {
    if ( pMyRemoteCharacteristic != NULL ) {
      if ( pMyRemoteCharacteristic->canWriteNoResponse() ) {

        //set colors
        writeRGB(0x7F, 0x00, 0x00); // Red
        writeRGB(0x00, 0x7F, 0x00); // Green
        writeRGB(0x00, 0x00, 0x7F); // Blue
        writeRGB(0x7F, 0x7F, 0x7F); // White

        //turn off
        on_off_data[2] = 0x00; 
        pMyRemoteCharacteristic->writeValue(on_off_data, sizeof(on_off_data));
        delay(500);

        //turn on
        on_off_data[2] = 0x01; 
        pMyRemoteCharacteristic->writeValue(on_off_data, sizeof(on_off_data));
        delay(500);
      }
    }
  }
}