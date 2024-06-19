#include "../include/utils.hpp"

#define UUID_BYTES 16

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
