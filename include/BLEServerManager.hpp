#ifndef BLE_SERVER_MANAGER_HPP
#define BLE_SERVER_MANAGER_HPP

#define DEBUG_MODE

#include <Arduino.h>
#include "BLEServer.h"
#include "BLEDevice.h"

class BLEServerManager {
public:
    BLEServerManager(const char* name);

    void addCharacteristic(const char* name, const char* defaultValue, BLECharacteristicCallbacks* callbacks);
    void start();

private:
    BLEServer* m_server;
    BLEService* m_service;

    class ServerCallBacks : public BLEServerCallbacks {
        void onConnect(BLEServer* pServer) {
#ifdef DEBUG_MODE
            Serial.println("Someone connected to us!");
#endif
        }

        void onDisconnect(BLEServer* pServer) {
#ifdef DEBUG_MODE
            Serial.println("Someone disconnected :(");
#endif
            pServer->startAdvertising();
        }
    };
};

#endif
