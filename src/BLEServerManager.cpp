#include <esp_bt_device.h>
#include "../include/BLEServerManager.hpp"
#include "../include/utils.hpp"

BLEServerManager::BLEServerManager(const char* name) {
    // Init server
    BLEDevice::init(name);
    m_server = BLEDevice::createServer();
    m_server->setCallbacks(new ServerCallBacks());

    // Create service
    const char* serviceUUID = generate_formatted_uuid(&esp_fill_random, esp_bt_dev_get_address());
    m_service = m_server->createService(serviceUUID);
}

void BLEServerManager::addCharacteristic(const char* name, const char* defaultValue, BLECharacteristicCallbacks* callbacks) {
    // Create characteristic in service
    const char* characteristicUUID = generate_formatted_uuid(&esp_fill_random, esp_bt_dev_get_address());
    BLECharacteristic* newCharacteristic = m_service->createCharacteristic(
                                           characteristicUUID,
                                           BLECharacteristic::PROPERTY_READ |
                                           BLECharacteristic::PROPERTY_WRITE
                                         );
    newCharacteristic->setCallbacks(callbacks);
    newCharacteristic->setValue(defaultValue);
    // Add a descriptor to the characteristic
    const char* descriptorUUID = generate_formatted_uuid(&esp_fill_random, esp_bt_dev_get_address());
    BLEDescriptor* newDescriptor = new BLEDescriptor(descriptorUUID);
    newDescriptor->setValue(name);
    newCharacteristic->addDescriptor(newDescriptor);
}

void BLEServerManager::start() {
    m_service->start();
    m_server->startAdvertising();

#if defined(DEBUG_MODE)
    Serial.print("Server address:");
    Serial.println(BLEDevice::getAddress().toString().c_str());
#endif
}
