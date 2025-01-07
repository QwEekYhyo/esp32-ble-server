#ifndef EEPROM_HPP
#define EEPROM_HPP

#include <cstdint>

void save_name(const char* buffer, uint16_t size);

void read_name(char* buffer);

#endif
