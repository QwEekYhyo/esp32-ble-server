#ifndef COLOR_HPP
#define COLOR_HPP

#include <cstdint>

class Color {
public:
    uint8_t r{0};
    uint8_t g{0};
    uint8_t b{0};

    Color() = default;
    Color(uint8_t red, uint8_t green, uint8_t blue);
    Color(const char* stringColor);

    void setColor(uint8_t red, uint8_t green, uint8_t blue);
    void setColor(const char*  stringColor);
    char* toString(char* buffer) const;

};

#endif
