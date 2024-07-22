#include <esp_bt_device.h>

#include "include/BLEServerManager.hpp"
#include "include/LEDManager.hpp"

#define FREQUENCY 2 // in Hz
#define DELAYED_SECONDS 1000 / FREQUENCY
#define DEFAULT_DEVICE_NAME "Procédure complexe"

LEDManager ledManager;

class NameCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        String value = pCharacteristic->getValue();
        if (value.length() > 0) {
            esp_ble_gap_set_device_name(value.c_str());
#ifdef DEBUG_MODE
            Serial.println("Changed device name");
#endif
        }
    }
};

class ColorCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        String colorString = pCharacteristic->getValue();
        if (colorString.length() == 6)
            ledManager.setColor(colorString.c_str());
    }
};

class BrightnessCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        int value = pCharacteristic->getValue().toInt();
        if (value <= 0) value = 1;
        else if (value > 255) value = 255;

        ledManager.setBrightness(value);
    }
};

class DistanceCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        int value = pCharacteristic->getValue().toInt();
        ledManager.displayDistance(value);

#ifdef DEBUG_MODE
        String stringValue = pCharacteristic->getValue();
        if (stringValue.length() > 0) {
            Serial.println("*********");
            Serial.print("New value: ");
            for (int i = 0; i < stringValue.length(); i++)
                Serial.print(stringValue[i]);
            Serial.println();
            Serial.println("*********");
        }
#endif
    }
};

void setup() {
    // Arduino specific soy dev things
#ifdef DEBUG_MODE
    Serial.begin(9600);
#endif
    ledManager.turnOff();

    BLEServerManager server(DEFAULT_DEVICE_NAME);

    server.addCharacteristic("Name", DEFAULT_DEVICE_NAME, new NameCallbacks());
    char colorString[7];
    server.addCharacteristic("Color", ledManager.getColor().toString(colorString), new ColorCallbacks());
    server.addCharacteristic("Brightness", "255", new BrightnessCallbacks());
    server.addCharacteristic("Distance", "0", new DistanceCallbacks());

    server.start();
}

void loop() {
    delay(DELAYED_SECONDS);
}
