#include <esp_bt_device.h>

#include "include/BLEServerManager.hpp"

#define LED_PIN 25

#define DEFAULT_DEVICE_NAME "Cucklord V3"

// TODO: debug mode

class NameCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        String value = pCharacteristic->getValue();
        if (value.length() > 0) {
            esp_ble_gap_set_device_name(value.c_str());
            Serial.println("Changed device name");
        }
    }
};

class BrightnessCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        int value = pCharacteristic->getValue().toInt();
        analogWrite(LED_PIN, value);
    }
};

class DistanceCallbacks : public BLECharacteristicCallbacks {
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

void setup() {
    // Arduino specific soy dev things
    Serial.begin(9600);
    pinMode(LED_PIN, OUTPUT);

    BLEServerManager server(DEFAULT_DEVICE_NAME);

    server.addCharacteristic("Name", DEFAULT_DEVICE_NAME, new NameCallbacks());
    server.addCharacteristic("Brightness", "0", new BrightnessCallbacks());
    server.addCharacteristic("Distance", "0", new DistanceCallbacks());

    server.start();
}

void loop() {
    delay(500);
}
