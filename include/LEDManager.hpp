#ifndef LED_MANAGER_HPP
#define LED_MANAGER_HPP

#include <Adafruit_NeoPixel.h>
#include <cstddef>
#include <cstdint>

#include "color.hpp"

class LEDManager {
public:
    /* Hardware Dependent */
    static constexpr uint8_t HEIGHT{1};
    /* Hardware Dependent */
    static constexpr uint8_t NUM_LED{21};

    static LEDManager& instance();
    LEDManager(const LEDManager&) = delete;
    LEDManager& operator=(const LEDManager&) = delete;

    void turnOff();
    void fill();
    void fill(const Color& color);
    void fillWithDelay(const Color& color, size_t delay);
    void emptyWithDelay(size_t delay, uint8_t index);
    void displayDistance(int distance);
    /* Hardware Dependent */
    void rainbow(int offset);
    /* Hardware Dependent */
    void line(uint8_t length);
    /* Hardware Dependent */
    void bluetoothWaiting(int offset);
    /* Hardware Dependent */
    void DEVICE_IS_ON();

    void setBrightness(uint8_t brightness);
    void setColor(const char* stringColor);
    const Color& getColor() const;

private:
    /* Hardware Dependent */
    static constexpr uint8_t LED_PIN{5};

    LEDManager();

    Adafruit_NeoPixel m_pixels;
    Color m_currentColor;
    size_t m_previousLineLength{0}; // this isn't used anymore but I might at some point

    /* Hardware Dependent */
    size_t getLedIndex(size_t x, size_t y) const;

};

#endif
