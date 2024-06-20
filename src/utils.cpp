#include "../include/utils.hpp"
#include <cstdio>

#define UUID_BYTES 16
#define UUID_CHARS (UUID_BYTES * 2) + 4

void generate_uuid(uint8_t* uuid, std::function<void(void*, size_t)> fill_random, const uint8_t* mac_address) {
    // Fill the first part of UUID with random numbers
    fill_random(uuid, 10);

    /*
        Because the UUID is generated with random numbers we need to specify
        that we are using version 4 here : xxxxxxxx-xxxx-4xxx-xxxx-xxxxxxxxxxxx
        so the sixth byte has to be between 64 and 79
    */
    uuid[6] = (uuid[6] & 0x0f) | 0x40;

    // Fill the last part of UUID with mac address
    for (uint8_t i = 0; i < 6; i++) {
        uuid[i + 10] = mac_address[i];
    }
}

#ifdef DEBUG_MODE
const char* format_uuid(const uint8_t* uuid) {
    char* result = new char[UUID_CHARS];
    int written = snprintf(result, UUID_CHARS + 1, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x", 
        uuid[0],
        uuid[1],
        uuid[2],
        uuid[3],
        uuid[4],
        uuid[5],
        uuid[6],
        uuid[7],
        uuid[8],
        uuid[9],
        uuid[10],
        uuid[11],
        uuid[12],
        uuid[13],
        uuid[14],
        uuid[15]
    );
    if (written != UUID_CHARS) {
        // ERROR
        return nullptr;
    }
    return result;
}

// UNUSED
const char* generate_formatted_uuid(uint8_t* uuid, std::function<void(void*, size_t)> fill_random, const uint8_t* mac_address) {
    generate_uuid(uuid, fill_random, mac_address);
    const char* result = format_uuid(uuid);
    return result;
}
#endif
