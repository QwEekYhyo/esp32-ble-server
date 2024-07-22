#include <Adafruit_NeoPixel.h>
#include <cstddef>
#include <cstdint>

#include "color.hpp"

class LEDManager {
public:
    static constexpr uint8_t HEIGHT{7};

    LEDManager();

    void turnOff();
    void fill();
    void fill(const Color& color);
    void displayDistance(int distance);
    /* Hardware Dependent */
    void rainbow(int offset);

    void setBrightness(uint8_t brightness);
    void setColor(const char* stringColor);
    const Color& getColor() const;

private:
    /* Hardware Dependent */
    static constexpr uint8_t LED_PIN{5};
    /* Hardware Dependent */
    static constexpr uint8_t NUM_LED{37};

    Adafruit_NeoPixel m_pixels;
    Color m_currentColor;
    uint8_t m_previousRadius{0};

    /* Hardware Dependent */
    size_t getLedIndex(size_t x, size_t y) const;
    /* Hardware Dependent */
    void circle(uint8_t radius);

};
