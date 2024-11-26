#include <esp_bt_device.h>
#include "../include/BLEServerManager.hpp"
#include "../include/utils.hpp"

BLEServerManager::BLEServerManager(const char* name) {
    // Init server
    BLEDevice::init(name);
    m_server = BLEDevice::createServer();
    m_server->setCallbacks(new ServerCallBacks());

    // Create service
    uint8_t buffer[16];
    generate_uuid(buffer, &esp_fill_random, esp_bt_dev_get_address());
    BLEUUID serviceUUID(buffer, 16, true);
    m_service = m_server->createService(serviceUUID);
}

BLECharacteristic* BLEServerManager::addCharacteristic(const char* name, const char* defaultValue, BLECharacteristicCallbacks* callbacks) {
    // Create characteristic in service
    uint8_t buffer[16];
    generate_uuid(buffer, &esp_fill_random, esp_bt_dev_get_address());
    BLEUUID characteristicUUID(buffer, 16, true);
    BLECharacteristic* newCharacteristic = m_service->createCharacteristic(
                                           characteristicUUID,
                                           BLECharacteristic::PROPERTY_READ |
                                           BLECharacteristic::PROPERTY_WRITE
                                         );
    newCharacteristic->setCallbacks(callbacks);
    newCharacteristic->setValue(defaultValue);
    // Add a descriptor to the characteristic
    generate_uuid(buffer, &esp_fill_random, esp_bt_dev_get_address());
    BLEUUID descriptorUUID(buffer, 16, true);
    BLEDescriptor* newDescriptor = new BLEDescriptor(descriptorUUID);
    newDescriptor->setAccessPermissions(ESP_GATT_PERM_READ);
    newDescriptor->setValue(name);
    newCharacteristic->addDescriptor(newDescriptor);

    return newCharacteristic;
}

void BLEServerManager::setBrightnessCharacteristic(BLECharacteristic* characteristic) {
    m_brightness = characteristic;
}

uint8_t BLEServerManager::getCurrentBrightness() const {
    int value = m_brightness->getValue().toInt();
    if (value <= 0) value = 1;
    else if (value > 255) value = 255;
    return value;
}

void BLEServerManager::start() {
    m_service->start();
    m_server->startAdvertising();
}

void BLEServerManager::stop() {
    m_service->stop();
    BLEDevice::stopAdvertising();
}
