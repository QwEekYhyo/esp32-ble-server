#ifndef BLE_SERVER_MANAGER_HPP
#define BLE_SERVER_MANAGER_HPP

#include <Arduino.h>
#include <cstdint>
#include "BLEServer.h"
#include "BLEDevice.h"

class BLEServerManager {
public:
    BLEServerManager(const char* name);

    BLECharacteristic* addCharacteristic(const char* name, const char* defaultValue, BLECharacteristicCallbacks* callbacks);
    void setBrightnessCharacteristic(BLECharacteristic*);
    uint8_t getCurrentBrightness() const;
    void start();
    void stop();

private:
    BLEServer* m_server;
    BLEService* m_service;
    BLECharacteristic* m_brightness;

    class ServerCallBacks : public BLEServerCallbacks {
        void onConnect(BLEServer* pServer) {}

        void onDisconnect(BLEServer* pServer) {
            pServer->startAdvertising();
        }
    };
};

#endif
