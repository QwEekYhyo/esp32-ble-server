#ifndef BLE_SERVER_MANAGER_HPP
#define BLE_SERVER_MANAGER_HPP

#include <Arduino.h>
#include "BLEServer.h"
#include "BLEDevice.h"

class BLEServerManager {
public:
    BLEServerManager(const char* name);

private:
    class ServerCallBacks : public BLEServerCallbacks {
        void onConnect(BLEServer* pServer) {
            Serial.println("Someone connected to us!");
        }

        void onDisconnect(BLEServer* pServer) {
            Serial.println("Someone disconnected :(");
            pServer->startAdvertising();
        }
    };
};

#endif
