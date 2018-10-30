/*
    Multi Sensors Bluetooth
    Author: Supachai Chaimangua (Tor)
    UpdatedDate: 2018-10-30T22:30:00+07:00
    CreatedDate: 2018-10-29T18:10:00+07:00

    Comment:

    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini

    Hardware: https://github.com/LilyGO/TTGO-T8-ESP32
*/

#include <Arduino.h>
#include <ArduinoJson.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <nvs.h>
#include <nvs_flash.h>

#define BLE_STATUS 2

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharacteristic;

bool bleConnected = false;
bool oldBleConnected = false;

uint8_t value = 0;


class ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
       digitalWrite(BLE_STATUS, HIGH);
       bleConnected = true;
       Serial.println("Bluetooth is conneted");
    };

    void onDisconnect(BLEServer* pServer) {
       digitalWrite(BLE_STATUS, LOW);
    }
};

class Callbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    
  }
};

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("Multi Sensors Bluetooth start...");
  
  pinMode(BLE_STATUS, OUTPUT); // Blue tooth
  pinMode(5, INPUT);
  pinMode(18, OUTPUT);
  pinMode(36, INPUT);
  BLEDevice::init("MultiSensors-Temperature");
  pServer = BLEDevice::createServer();

  pService = pServer->createService(SERVICE_UUID);

 pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE |
                                         BLECharacteristic::PROPERTY_NOTIFY |
                                         BLECharacteristic::PROPERTY_INDICATE
                                       );
  pCharacteristic->addDescriptor(new BLE2902());
  pServer->setCallbacks(new ServerCallbacks());
  // pCharacteristic->setCallbacks(new Callbacks());

  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}

void loop() {
  // notify changed value
  if (bleConnected) {
    value = analogRead(36);
    Serial.println(value);
    pCharacteristic->setValue(&value, 1);
    pCharacteristic->notify();
  }
  // disconnecting
  if (!bleConnected && oldBleConnected) {
      delay(500); // give the bluetooth stack the chance to get things ready
      pServer->startAdvertising(); // restart advertising
      Serial.println("Start advertising");
      oldBleConnected = bleConnected;
  }
  // connecting
  if (bleConnected && !oldBleConnected) {
      // do stuff here on connecting
      oldBleConnected = bleConnected;
  }
  delay(100);
}