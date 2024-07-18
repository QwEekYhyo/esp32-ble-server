#include "../include/color.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>

Color::Color(uint8_t red, uint8_t green, uint8_t blue)
    : r{red}, g{green}, b{blue} {}

// avoid using this constructor
Color::Color(const char* stringColor) {
    setColor(stringColor);
}

void Color::setColor(uint8_t red, uint8_t green, uint8_t blue) {
    r = red;
    g = green;
    b = blue;
}

void Color::setColor(const char* stringColor) {
    char currentColor[3];
    currentColor[2] = '\0';

    // Write red in currentColor
    memcpy(currentColor, stringColor, 2);
    r = std::strtol(currentColor, nullptr, 16);

    // Write green in currentColor
    memcpy(currentColor, stringColor + 2, 2);
    g = std::strtol(currentColor, nullptr, 16);

    // Write blue in currentColor
    memcpy(currentColor, stringColor + 4, 2);
    b = std::strtol(currentColor, nullptr, 16);
}

char* Color::toString(char* buffer) const {
    sprintf(buffer, "%02x%02x%02x", r, g, b);
    return buffer;
}
