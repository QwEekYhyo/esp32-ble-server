#include <esp_bt_device.h>
#include "Wire.h"

#include "include/BLEServerManager.hpp"
#include "include/LEDManager.hpp"
#include "include/callbacks.hpp"
#include "include/utils.hpp"

#define DEFAULT_DEVICE_NAME "Cool device v2"
#define BUTTON_PIN_BITMASK(GPIO) (1ULL << GPIO)  
uint64_t bitmask = (BUTTON_PIN_BITMASK(GPIO_NUM_10) | BUTTON_PIN_BITMASK(GPIO_NUM_11));

uint64_t lastBrightnessChange = 0;
bool isBrightnessChanging = false;
bool wasPreviouslyCharging = false;
int animOffset = 0;
BLEServerManager* server;

class BrightnessCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        int value = pCharacteristic->getValue().toInt();
        if (value <= 0) value = 1;
        else if (value > 255) value = 255;

        LEDManager::instance.setBrightness(value);
        isBrightnessChanging = true;
        lastBrightnessChange = millis64();
    }
};

void setup() {
    // I2C pins
    Wire.begin(13, 12);
    // Charger pin (to check if battery is charging)
    pinMode(10, INPUT);
    // Power button (to enable/disable deep sleep)
    pinMode(11, INPUT);
    LEDManager::instance.turnOff();

    server = new BLEServerManager(DEFAULT_DEVICE_NAME);

    server->addCharacteristic("Name", DEFAULT_DEVICE_NAME, new NameCallbacks());
    char colorString[7];
    server->addCharacteristic("Color", LEDManager::instance.getColor().toString(colorString), new ColorCallbacks());
    server->setBrightnessCharacteristic(server->addCharacteristic("Brightness", "50", new BrightnessCallbacks()));
    server->addCharacteristic("Distance", "0", new DistanceCallbacks());

    server->start();

    LEDManager::instance.fillWithDelay(Color(0, 10, 0), 80);
    LEDManager::instance.setBrightness(50);
    LEDManager::instance.turnOff();
}

uint8_t iterationCounter = 0;
int16_t V;
double voltage;

void loop() {
    Wire.beginTransmission(0x70);
    Wire.write(0);
    Wire.write(16);
    Wire.endTransmission();

    Wire.beginTransmission(0x70);
    Wire.write(8);
    Wire.endTransmission();
    Wire.requestFrom(0x70,2);
    V = Wire.read() | (Wire.read() << 8);

    voltage = float(V) * 0.00244;

    if (!digitalRead(10)) {
        /* Battery is charging */
        size_t line_length = (size_t) map_cool(voltage, 3.0, 4.2, 1.0, LEDManager::NUM_LED);
        if (!wasPreviouslyCharging) {
            wasPreviouslyCharging = true;
            LEDManager::instance.setColor("00FF00");
            LEDManager::instance.setBrightness(10);
            LEDManager::instance.line(LEDManager::NUM_LED);
            LEDManager::instance.emptyWithDelay(80, line_length);
            server->stop();
        }
        iterationCounter++;
        if (iterationCounter >= 25) {
            animOffset++;
            iterationCounter = 0;
        }
        LEDManager::instance.line(animOffset % 2 == 0 ? line_length : line_length - 1);
        delay(20);
    } else {
        /* Battery is not charging */
        if (wasPreviouslyCharging) {
            wasPreviouslyCharging = false;
            LEDManager::instance.turnOff();
            LEDManager::instance.setBrightness(server->getCurrentBrightness());
            server->start();
        }
        if (isBrightnessChanging) {
            iterationCounter++;
            if (iterationCounter >= 3) {
                animOffset++;
                iterationCounter = 0;
            }
            LEDManager::instance.rainbow(animOffset);
            delay(20);
            if (millis64() - lastBrightnessChange >= 1500) {
                isBrightnessChanging = false;
                LEDManager::instance.turnOff();
                animOffset = 0;
            }
        }
    }

    // Power button is pressed to turn off device
    if (!digitalRead(11)) {
        LEDManager::instance.turnOff();
        LEDManager::instance.setBrightness(50);
        LEDManager::instance.fillWithDelay(Color(100, 0, 0), 80);
        LEDManager::instance.turnOff();
        esp_sleep_enable_ext1_wakeup_io(bitmask, ESP_EXT1_WAKEUP_ANY_LOW);
        esp_deep_sleep_start();
    }
}
