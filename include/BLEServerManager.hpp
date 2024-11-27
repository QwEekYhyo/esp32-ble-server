#ifndef BLE_SERVER_MANAGER_HPP
#define BLE_SERVER_MANAGER_HPP

#include <Arduino.h>
#include <cstdint>
#include "BLEServer.h"
#include "BLEDevice.h"

class BLEServerManager {
public:
    BLEServerManager(const char* name);

    BLECharacteristic* addCharacteristic(const char* UUID, const char* defaultValue, BLECharacteristicCallbacks* callbacks);
    BLECharacteristic* addReadOnlyCharacteristic(const char* UUID, const char* defaultValue);
    void setBrightnessCharacteristic(BLECharacteristic*);
    void setBatteryCharacteristic(BLECharacteristic*);
    uint8_t getCurrentBrightness() const;
    void setCurrentBattery(double currentVoltage);
    void start();
    void stop();

private:
    BLEServer* m_server;
    BLEService* m_service;
    BLECharacteristic* m_brightness;
    BLECharacteristic* m_battery;

    class ServerCallBacks : public BLEServerCallbacks {
        void onConnect(BLEServer* pServer) {}

        void onDisconnect(BLEServer* pServer) {
            pServer->startAdvertising();
        }
    };
};

#endif
