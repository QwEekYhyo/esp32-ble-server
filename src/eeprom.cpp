#include "../include/eeprom.hpp"

#include <EEPROM.h>
#include <cstring>

void save_name(const char* buffer, uint16_t size) {
    if (size > 254) size = 254;
    EEPROM.write(1, size);
    for (uint16_t i = 0; i < size; i++)
        EEPROM.write(i + 2, buffer[i]);
    EEPROM.commit();
}

void read_name(char* buffer) {
    uint16_t stored_size = EEPROM.read(1);
    for (uint16_t i = 0; i < stored_size; i++)
        buffer[i] = EEPROM.read(i + 2);
    buffer[stored_size] = '\0';
}
