#include "../include/utils.hpp"
#include <cstdio>

#define UUID_BYTES 16
#define UUID_CHARS (UUID_BYTES * 2) + 4

const uint8_t* generate_uuid(std::function<void(void*, size_t)> fill_random, const uint8_t* mac_address) {
    // I don't know what happens if allocation fails
    uint8_t* uuid = new uint8_t[UUID_BYTES];

    // Fill the first part of UUID with random numbers
    fill_random(uuid, 10);

    /*
        Because the UUID is generated with random numbers we need to specify
        that we are using version 4 here : xxxxxxxx-xxxx-4xxx-xxxx-xxxxxxxxxxxx
        so the sixth bytes has to be between 64 and 79
        but because I don't know how to generate a number between a certain
        range using esp_random, I just set it to 64
    */
    uuid[6] = 64;

    // Fill the last part of UUID with mac address
    for (uint8_t i = 0; i < 6; i++) {
        uuid[i + 10] = mac_address[i];
    }

    return uuid;
}

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

const char* generate_formatted_uuid(std::function<void(void*, size_t)> fill_random, const uint8_t* mac_address) {
    const uint8_t* temp = generate_uuid(fill_random, mac_address);
    const char* result = format_uuid(temp);
    delete [] temp;
    return result;
}
