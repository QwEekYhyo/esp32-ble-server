#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include <functional>

const uint8_t* generate_uuid(std::function<void(void*, size_t)> fill_random, const uint8_t* mac_address);

#endif
