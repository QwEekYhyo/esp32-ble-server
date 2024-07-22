#include <cstdint>

#include "../include/LEDManager.hpp"

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

void LEDManager::fill() {
    fill(m_currentColor);
}

void LEDManager::fill(const Color& color) {
    for (uint8_t pixel = 0; pixel < LEDManager::NUM_LED; pixel++) {
        m_pixels.setPixelColor(pixel, color.r, color.g, color.b);
    }
    m_pixels.show();
}

void LEDManager::displayDistance(int distance) {
    if (distance <= 400) {
        uint8_t radius = 4 - distance / 100;
        if (radius != m_previousRadius) {
            for (uint8_t i = 0; i < 2; i++) {
                circle(radius);
                m_pixels.show();
                delay(100);
                turnOff();
                m_pixels.show();
                delay(100);
            }
        }
        circle(radius);
        m_pixels.show();
        m_previousRadius = radius;
    } else {
        turnOff();
        m_previousRadius = 0;
    }
}

void LEDManager::rainbow(int offset) {
    for (size_t x = 0; x < LEDManager::HEIGHT; x++) {
        int xOffset = (x + offset) % LEDManager::HEIGHT;
        for (size_t y = 0; y < shape[xOffset]; y++) {
            switch (x) {
                case 0:
                    m_pixels.setPixelColor(getLedIndex(xOffset, y), 255, 0, 0);
                    break;
                case 1:
                    m_pixels.setPixelColor(getLedIndex(xOffset, y), 255, 127, 0);
                    break;
                case 2:
                    m_pixels.setPixelColor(getLedIndex(xOffset, y), 255, 255, 0);
                    break;
                case 3:
                    m_pixels.setPixelColor(getLedIndex(xOffset, y), 0, 255, 0);
                    break;
                case 4:
                    m_pixels.setPixelColor(getLedIndex(xOffset, y), 0, 0, 255);
                    break;
                case 5:
                    m_pixels.setPixelColor(getLedIndex(xOffset, y), 75, 0, 130);
                    break;
                default:
                    m_pixels.setPixelColor(getLedIndex(xOffset, y), 148, 0, 211);
                    break;
            }
        }
    }

    m_pixels.show();
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
