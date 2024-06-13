#include "BLEDevice.h"
#include "BLEServer.h"

#define SERVICE_UUID        "1df96fce-7f73-4dfe-b126-89dfe643eeac"
#define CHARACTERISTIC_UUID "df449700-a753-4a1c-953a-d561c4848045"

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        String value = pCharacteristic->getValue();
        if (value.length() > 0) {
            Serial.println("*********");
            Serial.print("New value: ");
            for (int i = 0; i < value.length(); i++)
                Serial.print(value[i]);
            Serial.println();
            Serial.println("*********");
        }
    }
};

class ServerCallBacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        Serial.println("Someone connected to us!");
    }

    void onDisconnect(BLEServer* pServer) {
        Serial.println("Someone disconnected :(");
        pServer->startAdvertising();
    }
};

void setup() {
    Serial.begin(9600);
    BLEDevice::init("Cucklord V2");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallBacks());
    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                           CHARACTERISTIC_UUID,
                                           BLECharacteristic::PROPERTY_READ |
                                           BLECharacteristic::PROPERTY_WRITE
                                         );
    pCharacteristic->setCallbacks(new MyCallbacks());
    pCharacteristic->setValue("Hello World");
    pService->start();
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
    Serial.print("Server address:");
    Serial.println(BLEDevice::getAddress().toString().c_str());
}

void loop() {
    delay(500);
}
