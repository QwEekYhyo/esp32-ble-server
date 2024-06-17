#include "BLEDevice.h"
#include "BLEServer.h"

#define LED_PIN 25

#define DEFAULT_DEVICE_NAME "Cucklord V3"

#define SERVICE_UUID                   "1df96fce-7f73-4dfe-b126-89dfe643eeac"
#define NAME_CHARACTERISTIC_UUID       "d3139fcc-c344-45c4-a370-e32af8de79c0"
#define NAME_DESCRIPTOR_UUID           "69344e7f-9d3a-465e-aa34-3cc2f5dbdb76"
#define DISTANCE_CHARACTERISTIC_UUID   "df449700-a753-4a1c-953a-d561c4848045"
#define DISTANCE_DESCRIPTOR_UUID       "3c462cba-886d-41f5-8fe6-37801a5bf590"
#define BRIGHTNESS_CHARACTERISTIC_UUID "535ba877-fc4d-4059-88c1-56c081787162"
#define BRIGHTNESS_DESCRIPTOR_UUID     "9e9c5424-d371-4fb9-b811-c466d765884b"

class NameCallback : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        String value = pCharacteristic->getValue();
        if (value.length() > 0) {
            esp_ble_gap_set_device_name(value.c_str());
            Serial.println("Changed device name");
        }
    }
};

class BrightnessCallback : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        int value = pCharacteristic->getValue().toInt();
        analogWrite(LED_PIN, value);
    }
};

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
    // Arduino specific soy dev things
    Serial.begin(9600);
    pinMode(LED_PIN, OUTPUT);

    // Init server
    BLEDevice::init(DEFAULT_DEVICE_NAME);
    BLEServer* pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallBacks());

    // Create service
    BLEService* pService = pServer->createService(SERVICE_UUID);

    // ================== Name ==================
    // Create characteristic in service
    BLECharacteristic* nameCharacteristic = pService->createCharacteristic(
                                           NAME_CHARACTERISTIC_UUID,
                                           BLECharacteristic::PROPERTY_READ |
                                           BLECharacteristic::PROPERTY_WRITE
                                         );
    nameCharacteristic->setCallbacks(new NameCallback());
    nameCharacteristic->setValue(DEFAULT_DEVICE_NAME);
    // Add a descriptor to the characteristic
    BLEDescriptor* nameDescriptor = new BLEDescriptor(NAME_DESCRIPTOR_UUID);
    nameDescriptor->setValue("Name");
    nameCharacteristic->addDescriptor(nameDescriptor);

    // ================== Distance ==================
    // Create characteristic in service
    BLECharacteristic* distanceCharacteristic = pService->createCharacteristic(
                                           DISTANCE_CHARACTERISTIC_UUID,
                                           BLECharacteristic::PROPERTY_READ |
                                           BLECharacteristic::PROPERTY_WRITE
                                         );
    distanceCharacteristic->setCallbacks(new MyCallbacks());
    distanceCharacteristic->setValue("Hello World");
    // Add a descriptor to the characteristic
    BLEDescriptor* distanceDescriptor = new BLEDescriptor(DISTANCE_DESCRIPTOR_UUID);
    distanceDescriptor->setValue("Distance");
    distanceCharacteristic->addDescriptor(distanceDescriptor);

    // ================== Brightness ==================
    // Create characteristic in service
    BLECharacteristic* brightnessCharacteristic = pService->createCharacteristic(
                                           BRIGHTNESS_CHARACTERISTIC_UUID,
                                           BLECharacteristic::PROPERTY_READ |
                                           BLECharacteristic::PROPERTY_WRITE
                                         );
    brightnessCharacteristic->setCallbacks(new BrightnessCallback());
    brightnessCharacteristic->setValue("Hello World");
    // Add a descriptor to the characteristic
    BLEDescriptor* brightnessDescriptor = new BLEDescriptor(BRIGHTNESS_DESCRIPTOR_UUID);
    brightnessDescriptor->setValue("Brightness");
    brightnessCharacteristic->addDescriptor(brightnessDescriptor);
    // ====================================

    // Start everything
    pService->start();
    pServer->startAdvertising();

    Serial.print("Server address:");
    Serial.println(BLEDevice::getAddress().toString().c_str());
}

void loop() {
    delay(500);
}
