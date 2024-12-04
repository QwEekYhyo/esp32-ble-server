#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include <functional>

void generate_uuid(uint8_t* uuid, std::function<void(void*, size_t)> fill_random, const uint8_t* mac_address);
#ifdef DEBUG_MODE
const char* format_uuid(const uint8_t* uuid);
const char* generate_formatted_uuid(uint8_t* uuid, std::function<void(void*, size_t)> fill_random, const uint8_t* mac_address);
#endif

namespace Nutils {
    template <typename T>
    inline T map(T x, T in_min, T in_max, T out_min, T out_max) {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    template <typename T>
    inline T clamp(T x, T min, T max) {
        return x <= min ? min : (x >= max ? max : x);
    }
}

uint64_t millis64();

#endif
