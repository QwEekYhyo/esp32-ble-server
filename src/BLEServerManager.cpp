#include <esp_bt_device.h>
#include "../include/BLEServerManager.hpp"
#include "../include/utils.hpp"

BLEServerManager::BLEServerManager(const char* name) {
    // Init server
    BLEDevice::init(name);
    BLEServer* pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallBacks());

    // Create service
    const char* serviceUUID = generate_formatted_uuid(&esp_fill_random, esp_bt_dev_get_address());
    BLEService* pService = pServer->createService(serviceUUID);

    pService->start();
    pServer->startAdvertising();

    Serial.print("Server address:");
    Serial.println(BLEDevice::getAddress().toString().c_str());
}
