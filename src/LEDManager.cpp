#include <cmath>
#include <cstdint>

#include "../include/LEDManager.hpp"

LEDManager LEDManager::instance;

void hue_to_rgb(float hue, uint8_t *r, uint8_t *g, uint8_t *b) {
    float s = 1.0f, v = 1.0f;  // Full saturation and value
    int i = floor(hue * 6);
    float f = hue * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    switch(i % 6) {
        case 0: *r = v * 255; *g = t * 255; *b = p * 255; break;
        case 1: *r = q * 255; *g = v * 255; *b = p * 255; break;
        case 2: *r = p * 255; *g = v * 255; *b = t * 255; break;
        case 3: *r = p * 255; *g = q * 255; *b = v * 255; break;
        case 4: *r = t * 255; *g = p * 255; *b = v * 255; break;
        case 5: *r = v * 255; *g = p * 255; *b = q * 255; break;
    }
}

uint8_t shape[LEDManager::HEIGHT] = {
    21,
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

void LEDManager::fillWithDelay(const Color& color, size_t millis) {
    for (uint8_t pixel = 0; pixel < LEDManager::NUM_LED; pixel++) {
        m_pixels.setPixelColor(pixel, color.r, color.g, color.b);
        m_pixels.show();
        delay(millis);
    }
}

void LEDManager::displayDistance(int distance) {
    if (distance <= 400) {
        uint8_t line_length = 1 + ((400 - distance) / 400.0) * 20.0;
        line(line_length);
    } else {
        turnOff();
        m_previousLineLength = 0;
    }
}

void LEDManager::rainbow(int offset) {
    for (int y = 0; y < LEDManager::HEIGHT; y++) {
        for (int x = 0; x < shape[y]; x++) {
            int xOffset = (x + offset) % shape[y];
            float hue = fmod(((float) x / shape[y]) + offset, 1.0f);
            uint8_t r, g, b;
            hue_to_rgb(hue, &r, &g, &b);
            m_pixels.setPixelColor(getLedIndex(xOffset, y), r, g, b);
        }
    }

    m_pixels.show();
}

size_t LEDManager::getLedIndex(size_t x, size_t y) const {
    if (y >= LEDManager::HEIGHT || x >= shape[y])
        return 0;
    return y * shape[y] + x;
}

void LEDManager::line(uint8_t length) {
    if (length == 0) {
        for (uint8_t i = 0; i < LEDManager::NUM_LED; i++) {
            m_pixels.setPixelColor(i, 0, 0, 0);
        }
    } else {
        for (uint8_t y = 0; y < LEDManager::HEIGHT; y++) {
            for (uint8_t x = 0; x < shape[y]; x++) {
                if (x < length) {
                    m_pixels.setPixelColor(getLedIndex(x, y), m_currentColor.r, m_currentColor.g, m_currentColor.b);
                } else {
                    m_pixels.setPixelColor(getLedIndex(x, y), 0, 0, 0);
                }
            }
        }
    }
    m_pixels.show();
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
