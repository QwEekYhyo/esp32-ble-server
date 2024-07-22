#include <Adafruit_NeoPixel.h>
#include <cstddef>
#include <cstdint>

#include "color.hpp"

class LEDManager {
public:
    static constexpr uint8_t HEIGHT{7};

    LEDManager();

    void turnOff();
    void displayDistance(int distance);

    void setBrightness(uint8_t brightness);
    void setColor(const char* stringColor);
    const Color& getColor() const;

private:
    static constexpr uint8_t LED_PIN{5};
    static constexpr uint8_t NUM_LED{37};

    Adafruit_NeoPixel m_pixels;
    Color m_currentColor;

    size_t getLedIndex(size_t x, size_t y) const;
    void circle(uint8_t radius);

};
