#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include <functional>

void generate_uuid(uint8_t* uuid, std::function<void(void*, size_t)> fill_random, const uint8_t* mac_address);
#ifdef DEBUG_MODE
const char* format_uuid(const uint8_t* uuid);
const char* generate_formatted_uuid(uint8_t* uuid, std::function<void(void*, size_t)> fill_random, const uint8_t* mac_address);
#endif

uint64_t millis64();
float map_cool(float x, float in_min, float in_max, float out_min, float out_max);

#endif
