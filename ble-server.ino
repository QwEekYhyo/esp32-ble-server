#include "BLEDevice.h"
#include "BLEServer.h"

#define DEFAULT_DEVICE_NAME "Cucklord V3"

#define SERVICE_UUID        "1df96fce-7f73-4dfe-b126-89dfe643eeac"
#define CHARACTERISTIC_UUID "df449700-a753-4a1c-953a-d561c4848045"
#define DESCRIPTOR_UUID     "3c462cba-886d-41f5-8fe6-37801a5bf590"

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
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
    // Init server
    BLEDevice::init(DEFAULT_DEVICE_NAME);
    // esp_ble_gap_set_device_name(DEFAULT_DEVICE_NAME); // <-- Method to change device name I think
    BLEServer* pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallBacks());

    // Create service
    BLEService* pService = pServer->createService(SERVICE_UUID);

    // Create characteristic in service
    BLECharacteristic* pCharacteristic = pService->createCharacteristic(
                                           CHARACTERISTIC_UUID,
                                           BLECharacteristic::PROPERTY_READ |
                                           BLECharacteristic::PROPERTY_WRITE
                                         );
    pCharacteristic->setCallbacks(new MyCallbacks());
    pCharacteristic->setValue("Hello World");
    // Add a descriptor to the characteristic
    BLEDescriptor* pDescriptor = new BLEDescriptor(DESCRIPTOR_UUID);
    pDescriptor->setValue("Distance");
    pCharacteristic->addDescriptor(pDescriptor);

    // Start everything
    pService->start();
    pServer->startAdvertising();

    Serial.print("Server address:");
    Serial.println(BLEDevice::getAddress().toString().c_str());
}

void loop() {
    delay(500);
}
