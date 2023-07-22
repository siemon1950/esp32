/*******************************************************************************
 * Sketch name: BLE with one-way Serial communication
 * Description: 
 * Created on:  February 2023
 * Author:      Neil Cameron
 * Book:        ESP32 Communication
 * Chapter :    5 - BLE beacons
 ******************************************************************************/

#include <BLEDevice.h>          // include BLE library
#include <BLE2902.h>          //    and BLE2902 library
BLEServer * Server;         // define BLE server,
BLEService * Service;         //    service, 
BLECharacteristic * Char;       //    characteristic
BLEDescriptor * Desc;         //    and descriptor
BLE2902 * Notfy;
char ServUUID[] = "6e400001-b5a3-f393-e0a9-e50e24dcca9e"; // UUIDs
char CharUUID[] = "6e400003-b5a3-f393-e0a9-e50e24dcca9e";
int count = 0, clientCon = 0;
String str;
char msg[24];           // char array to store message

class ServerCallback : public BLEServerCallbacks  // callback function
{
  void onConnect(BLEServer * Server)    // client connect to server
  {
    clientCon = 1;
    Serial.println("connected to client");
    delay(500);
  }
  void onDisconnect(BLEServer * Server)   // client disconnect from server
  {
    clientCon = 0;
    delay(500);
    Server->startAdvertising();     // server start advertising
    Serial.println("client disconnected\nstarted advertising");
  }
};

void setup()
{
  Serial.begin(115200);
  BLEDevice::init("ESP32");       // define BLE device name,
  Server = BLEDevice::createServer();
  Server->setCallbacks(new ServerCallback());
  Service = Server->createService(ServUUID);    // add service to server
  Char = Service->createCharacteristic(CharUUID,    // add characteristic to server
           BLECharacteristic::PROPERTY_NOTIFY);
  Desc = new BLE2902();
  Char->addDescriptor(Desc);        // add descriptor to characteristic
  Notfy = (BLE2902*)Desc;       // link notifier to descriptor
  Service->start();
  Server->getAdvertising()->start();    // start advertising services
}

void loop()
{
  if(clientCon == 1)          // client connected to server
  {
    count++;
    str = "counts "+String(count)+","+String(0.5*count); // create message
    str.toCharArray(msg, 24);       // convert to character array
    Char->setValue(msg);        // set characteristic to message
    Char->notify();
    Serial.printf("notify %d msg %s \n", 
                   Notfy->getNotifications(), msg);
    delay(5000);
  }
}
