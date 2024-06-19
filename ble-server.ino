#include <esp_bt_device.h>

#include "include/BLEServerManager.hpp"

#define LED_PIN 25

#define DEFAULT_DEVICE_NAME "Cucklord V3"

void setup() {
    // Arduino specific soy dev things
    Serial.begin(9600);
    pinMode(LED_PIN, OUTPUT);

    BLEServerManager test(DEFAULT_DEVICE_NAME);
}

void loop() {
    delay(500);
}
