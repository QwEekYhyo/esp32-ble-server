#ifndef BLE_SERVER_MANAGER_HPP
#define BLE_SERVER_MANAGER_HPP

#define DEFAULT_DEVICE_NAME "CopBuster v2"

#include <Arduino.h>
#include <cstdint>
#include "BLEServer.h"
#include "BLEDevice.h"

class BLEServerManager {
public:
    static BLEServerManager* instance();
    BLEServerManager(const BLEServerManager&) = delete;
    BLEServerManager& operator=(const BLEServerManager&) = delete;

    BLECharacteristic* addCharacteristic(const char* UUID, const char* defaultValue, BLECharacteristicCallbacks* callbacks);
    BLECharacteristic* addReadOnlyCharacteristic(const char* UUID, const char* defaultValue);
    void setBrightnessCharacteristic(BLECharacteristic*);
    void setBatteryCharacteristic(BLECharacteristic*);
    uint8_t getCurrentBrightness() const;
    void setCurrentBattery(double currentVoltage);
    uint8_t connected() const;

    void start();
    void stop();

private:
    static BLEServerManager* m_instance;

    BLEServerManager();

    volatile uint8_t m_connected{0};
    BLEServer* m_server;
    BLEService* m_service;
    BLECharacteristic* m_brightness;
    BLECharacteristic* m_battery;

    class ServerCallBacks : public BLEServerCallbacks {
        void onConnect(BLEServer* pServer) {
            BLEServerManager::instance()->m_connected = 1;
        }

        void onDisconnect(BLEServer* pServer) {
            BLEServerManager::instance()->m_connected = 0;
            pServer->startAdvertising();
        }
    };
};

#endif
