#include <esp_bt_device.h>
#include "../include/BLEServerManager.hpp"
#include "../include/utils.hpp"

BLEServerManager* BLEServerManager::m_instance = nullptr;
BLEServerManager* BLEServerManager::instance() {
    if (!BLEServerManager::m_instance)
        BLEServerManager::m_instance = new BLEServerManager();
    return BLEServerManager::m_instance;
}

BLEServerManager::BLEServerManager() {
    // Init server
    BLEDevice::init(DEFAULT_DEVICE_NAME);
    m_server = BLEDevice::createServer();
    m_server->setCallbacks(new ServerCallBacks());

    // Create service
    uint8_t buffer[16];
    generate_uuid(buffer, &esp_fill_random, esp_bt_dev_get_address());
    BLEUUID serviceUUID(buffer, 16, true);
    m_service = m_server->createService(serviceUUID);
}

BLECharacteristic* BLEServerManager::addCharacteristic(const char* UUID, const char* defaultValue, BLECharacteristicCallbacks* callbacks) {
    // Create characteristic in service
    BLECharacteristic* newCharacteristic = m_service->createCharacteristic(
                                           UUID,
                                           BLECharacteristic::PROPERTY_READ |
                                           BLECharacteristic::PROPERTY_WRITE
                                         );
    newCharacteristic->setCallbacks(callbacks);
    newCharacteristic->setValue(defaultValue);
    return newCharacteristic;
}

BLECharacteristic* BLEServerManager::addReadOnlyCharacteristic(const char* UUID, const char* defaultValue) {
    // Create characteristic in service
    BLECharacteristic* newCharacteristic = m_service->createCharacteristic(
                                           UUID,
                                           BLECharacteristic::PROPERTY_READ
                                         );
    newCharacteristic->setValue(defaultValue);
    return newCharacteristic;
}

void BLEServerManager::setBrightnessCharacteristic(BLECharacteristic* characteristic) {
    m_brightness = characteristic;
}

void BLEServerManager::setBatteryCharacteristic(BLECharacteristic* characteristic) {
    m_battery = characteristic;
}

uint8_t BLEServerManager::getCurrentBrightness() const {
    int value = m_brightness->getValue().toInt();
    if (value <= 0) value = 1;
    else if (value > 255) value = 255;
    return value;
}

void BLEServerManager::setCurrentBattery(double currentVoltage) {
    currentVoltage = map_cool(currentVoltage, 3.0, 4.2, 0, 100);
    int batteryPercentage = floor(currentVoltage);
    m_battery->setValue(String(batteryPercentage));
}

void BLEServerManager::start() {
    m_service->start();
    m_server->startAdvertising();
}

void BLEServerManager::stop() {
    m_service->stop();
    BLEDevice::stopAdvertising();
}
