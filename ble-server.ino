#include <esp_bt_device.h>
#include <EEPROM.h>
#include "Wire.h"

#include "include/BLEServerManager.hpp"
#include "include/LEDManager.hpp"
#include "include/callbacks.hpp"
#include "include/eeprom.hpp"
#include "include/utils.hpp"

#define BATTERY_UUID    "11111111-1111-1111-1111-111111111111"
#define BRIGHTNESS_UUID "22222222-2222-2222-2222-222222222222"
#define COLOR_UUID      "33333333-3333-3333-3333-333333333333"
#define DISTANCE_UUID   "44444444-4444-4444-4444-444444444444"
#define NAME_UUID       "55555555-5555-5555-5555-555555555555"

#define BUTTON_PIN_BITMASK(GPIO) (1ULL << GPIO)  
static constexpr uint64_t bitmask = (BUTTON_PIN_BITMASK(GPIO_NUM_10) | BUTTON_PIN_BITMASK(GPIO_NUM_11));

static constexpr float MIN_BATTERY_VOLTAGE = 3.0;
static constexpr float MAX_BATTERY_VOLTAGE = 4.2;
static constexpr uint64_t BUTTON_HOLD_TIME = 2300;

static constexpr uint16_t EEPROM_SIZE = 512;

volatile uint64_t lastBrightnessChange = 0;
volatile bool isBrightnessChanging = false;
uint64_t buttonPressStartTime = 0;
unsigned int animOffset = 0;
uint8_t iterationCounter = 0;
bool wasPreviouslyCharging = false;
bool wasPreviouslyConnected = false;

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

    // Setup Bluetooth Low Energy server
    EEPROM.begin(EEPROM_SIZE);
    // If EEPROM was never written
    if (EEPROM.read(0) == 0xff) {
        EEPROM.write(0, 0x69);
        save_name(DEFAULT_DEVICE_NAME, strlen(DEFAULT_DEVICE_NAME));
    }
    char name_str[256];
    read_name(name_str);
    esp_ble_gap_set_device_name(name_str);
    server->addCharacteristic(NAME_UUID, name_str, new NameCallbacks());
    char colorString[7];
    server->addCharacteristic(COLOR_UUID, ledManager.getColor().toString(colorString), new ColorCallbacks());
    server->addCharacteristic(DISTANCE_UUID, "0", new DistanceCallbacks());
    server->setBrightnessCharacteristic(server->addCharacteristic(BRIGHTNESS_UUID, "50", new BrightnessCallbacks()));
    server->setBatteryCharacteristic(server->addReadOnlyCharacteristic(BATTERY_UUID, "0"));

    server->start();

    // Power on animation
    ledManager.fillWithDelay(Color(0, 10, 0), 80);
    ledManager.setBrightness(50);
    ledManager.turnOff();
}

double voltage; // Calculated voltage in Volts
int16_t V; // Raw reading from magic module

void loop() {
    BLEServerManager* server = BLEServerManager::instance();
    LEDManager& ledManager = LEDManager::instance();

    iterationCounter++;

    // Read magic module to get battery voltage
    Wire.beginTransmission(0x70);
    Wire.write(0);
    Wire.write(16);
    Wire.endTransmission();

    Wire.beginTransmission(0x70);
    Wire.write(8);
    Wire.endTransmission();
    Wire.requestFrom(0x70, 2);
    V = Wire.read() | (Wire.read() << 8);

    voltage = float(V) * 0.00244;
    if (iterationCounter % 15 <= 3)
        server->setCurrentBattery(voltage);

    if (!digitalRead(10) && !server->connected()) {
        /* Battery is charging */
        /* Not connected to any phone */
        voltage = Nutils::clamp<float>(voltage, MIN_BATTERY_VOLTAGE, MAX_BATTERY_VOLTAGE);
        uint8_t line_length = (uint8_t) Nutils::map<float>(
                voltage,
                MIN_BATTERY_VOLTAGE,
                MAX_BATTERY_VOLTAGE,
                1.0,
                LEDManager::NUM_LED
        );
        // All of this shit is just the charging animation
        if (!wasPreviouslyCharging) {
            wasPreviouslyCharging = true;
            wasPreviouslyConnected = false;
            ledManager.setColor("00FF00");
            ledManager.setBrightness(10);
            ledManager.line(LEDManager::NUM_LED);
            ledManager.emptyWithDelay(80, line_length);
        }
        if (iterationCounter >= 25) {
            animOffset++;
            iterationCounter = 0;
        }
        ledManager.line(animOffset % 2 == 0 ? line_length : line_length - 1);
        delay(20);
    } else {
        /* Battery is not charging */
        /* or it is, but also connected to a phone */

        // Clean charging animation
        if (wasPreviouslyCharging) {
            wasPreviouslyCharging = false;
            ledManager.turnOff();
            ledManager.setBrightness(server->getCurrentBrightness());
        }

        // Waiting for Bluetooth connection animation
        if (!server->connected()) {
            if (wasPreviouslyConnected) {
                wasPreviouslyConnected = false;
                ledManager.setBrightness(50);
            }

            static uint64_t lastAnimationTime = 0;
            if (millis64() - lastAnimationTime >= 20) {
                lastAnimationTime = millis64();
                ledManager.bluetoothWaiting(lastAnimationTime);
            }
        } else if (!wasPreviouslyConnected) {
            wasPreviouslyConnected = true;
            ledManager.setBrightness(server->getCurrentBrightness());
            ledManager.resetCursors();
            ledManager.DEVICE_IS_ON();
        }

        // Brightness changing animation
        if (isBrightnessChanging) {
            if (iterationCounter >= 30) {
                animOffset++;
                iterationCounter = 0;
            }
            ledManager.rainbow(animOffset);
            if (millis64() - lastBrightnessChange >= 1500) {
                isBrightnessChanging = false;
                ledManager.DEVICE_IS_ON();
                animOffset = 0;
            }
        } else if (server->connected()) {
            ledManager.reachTargetCursor();
        }
    }

    // Check if the power button is pressed
    if (!digitalRead(11)) {
        if (buttonPressStartTime == 0) {
            buttonPressStartTime = millis64();
        } else if (millis64() - buttonPressStartTime >= BUTTON_HOLD_TIME) {
            // Button held for BUTTON_HOLD_TIME ms
            ledManager.turnOff();
            server->stop();
            ledManager.setBrightness(50);
            ledManager.fillWithDelay(Color(100, 0, 0), 80);
            ledManager.turnOff();
            esp_sleep_enable_ext1_wakeup_io(bitmask, ESP_EXT1_WAKEUP_ANY_LOW);
            esp_deep_sleep_start();
        }
    } else {
        buttonPressStartTime = 0; // Reset if the button is released
    }

    // If these counters are near their max value
    // set them to zero to avoid overflow
    if (iterationCounter >= 253)
        iterationCounter = 0;
    if (animOffset >= 4294967200) // lmao
        animOffset = 0;
}

