#include <esp_bt_device.h>
#include "Wire.h"

#include "include/BLEServerManager.hpp"
#include "include/LEDManager.hpp"
#include "include/callbacks.hpp"
#include "include/utils.hpp"

#define BATTERY_UUID    "11111111-1111-1111-1111-111111111111"
#define BRIGHTNESS_UUID "22222222-2222-2222-2222-222222222222"
#define COLOR_UUID      "33333333-3333-3333-3333-333333333333"
#define DISTANCE_UUID   "44444444-4444-4444-4444-444444444444"
#define NAME_UUID       "55555555-5555-5555-5555-555555555555"

#define BUTTON_PIN_BITMASK(GPIO) (1ULL << GPIO)  
uint64_t bitmask = (BUTTON_PIN_BITMASK(GPIO_NUM_10) | BUTTON_PIN_BITMASK(GPIO_NUM_11));

uint64_t lastBrightnessChange = 0;
bool isBrightnessChanging = false;
bool wasPreviouslyCharging = false;
int animOffset = 0;

class BrightnessCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        int value = pCharacteristic->getValue().toInt();
        if (value <= 0) value = 1;
        else if (value > 255) value = 255;

        LEDManager::instance().setBrightness(value);
        isBrightnessChanging = true;
        lastBrightnessChange = millis64();
    }
};

void setup() {
    BLEServerManager* server = BLEServerManager::instance();
    LEDManager& ledManager = LEDManager::instance();
    // I2C pins
    Wire.begin(13, 12);
    // Charger pin (to check if battery is charging)
    pinMode(10, INPUT);
    // Power button (to enable/disable deep sleep)
    pinMode(11, INPUT);
    ledManager.turnOff();

    server->addCharacteristic(NAME_UUID, DEFAULT_DEVICE_NAME, new NameCallbacks());
    char colorString[7];
    server->addCharacteristic(COLOR_UUID, ledManager.getColor().toString(colorString), new ColorCallbacks());
    server->addCharacteristic(DISTANCE_UUID, "0", new DistanceCallbacks());
    server->setBrightnessCharacteristic(server->addCharacteristic(BRIGHTNESS_UUID, "50", new BrightnessCallbacks()));
    server->setBatteryCharacteristic(server->addReadOnlyCharacteristic(BATTERY_UUID, "0"));

    server->start();

    ledManager.fillWithDelay(Color(0, 10, 0), 80);
    ledManager.setBrightness(50);
    ledManager.turnOff();
}

uint8_t iterationCounter = 0;
int16_t V;
double voltage;

void loop() {
    BLEServerManager* server = BLEServerManager::instance();
    LEDManager& ledManager = LEDManager::instance();

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
    if (iterationCounter % 10 == 0)
        server->setCurrentBattery(voltage);

    if (!digitalRead(10)) {
        /* Battery is charging */
        size_t line_length = (size_t) map_cool(voltage, 3.0, 4.2, 1.0, LEDManager::NUM_LED);
        if (!wasPreviouslyCharging) {
            wasPreviouslyCharging = true;
            ledManager.setColor("00FF00");
            ledManager.setBrightness(10);
            ledManager.line(LEDManager::NUM_LED);
            ledManager.emptyWithDelay(80, line_length);
            server->stop();
        }
        iterationCounter++;
        if (iterationCounter >= 25) {
            animOffset++;
            iterationCounter = 0;
        }
        ledManager.line(animOffset % 2 == 0 ? line_length : line_length - 1);
        delay(20);
    } else {
        /* Battery is not charging */
        if (wasPreviouslyCharging) {
            wasPreviouslyCharging = false;
            ledManager.turnOff();
            ledManager.setBrightness(server->getCurrentBrightness());
            server->start();
        }
        if (isBrightnessChanging) {
            iterationCounter++;
            if (iterationCounter >= 3) {
                animOffset++;
                iterationCounter = 0;
            }
            ledManager.rainbow(animOffset);
            delay(20);
            if (millis64() - lastBrightnessChange >= 1500) {
                isBrightnessChanging = false;
                ledManager.turnOff();
                animOffset = 0;
            }
        }
    }

    // Power button is pressed to turn off device
    if (!digitalRead(11)) {
        ledManager.turnOff();
        server->stop();
        ledManager.setBrightness(50);
        ledManager.fillWithDelay(Color(100, 0, 0), 80);
        ledManager.turnOff();
        esp_sleep_enable_ext1_wakeup_io(bitmask, ESP_EXT1_WAKEUP_ANY_LOW);
        esp_deep_sleep_start();
    }
}
