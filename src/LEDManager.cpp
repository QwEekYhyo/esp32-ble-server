#include "../include/LEDManager.hpp"
#include <cstdint>

uint8_t shape[LEDManager::HEIGHT] = {
    4,
    5,
    6,
    7,
    6,
    5,
    4,
};

LEDManager::LEDManager()
    : m_pixels(LEDManager::NUM_LED, LEDManager::LED_PIN), m_currentColor(255, 0, 0) {
    m_pixels.begin();
}

void LEDManager::turnOff() {
    for (uint8_t pixel = 0; pixel < LEDManager::NUM_LED; pixel++) {
        m_pixels.setPixelColor(pixel, 0, 0, 0);
    }
    m_pixels.show();
}

void LEDManager::displayDistance(int distance) {
    if (distance <= 400) {
        circle(4 - distance / 100);
        m_pixels.show();
    } else {
        turnOff();
    }
}

size_t LEDManager::getLedIndex(size_t x, size_t y) const {
    size_t result = 0;
    for (size_t row = x + 1; row < LEDManager::HEIGHT; row++) {
        result += shape[row];
    }
    return result + shape[x] - y - 1;
}

void LEDManager::circle(uint8_t radius) {
    if (radius == 0) {
        for (size_t i = 0; i < LEDManager::NUM_LED; i++) {
            m_pixels.setPixelColor(i, 0, 0, 0);
        }
    } else {
        uint8_t outerRadius = 4 - radius;
        for (size_t x = 0; x < LEDManager::HEIGHT; x++) {
            for (size_t y = 0; y < shape[x]; y++) {
                if (x >= outerRadius && x < LEDManager::HEIGHT - outerRadius && y >= outerRadius && y < shape[x] - outerRadius) {
                    m_pixels.setPixelColor(getLedIndex(x, y), m_currentColor.r, m_currentColor.g, m_currentColor.b);
                } else {
                    m_pixels.setPixelColor(getLedIndex(x, y), 0, 0, 0);
                }
            }
        }
    }
}

void LEDManager::setBrightness(uint8_t brightness) {
    m_pixels.setBrightness(brightness);
}

void LEDManager::setColor(const char* stringColor) {
    m_currentColor.setColor(stringColor);
}

const Color& LEDManager::getColor() const {
    return m_currentColor;
}
