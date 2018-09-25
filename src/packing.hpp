/**
 * @file packing.hpp
 * @brief Definitions for packing and unpacking primitive types (and strings)
 * @author Giovanni Curiel dos Santos (giovannicuriel@gmail.com)
 * @version 0.0.1-alpha.1
 * @date 24/09/2018
 * @copyright GPL-3.0
 */

#ifndef _PACKING_HPP__
#define _PACKING_HPP__

#include <stdint.h>
#include <string>

typedef uint8_t byte;

void pack(byte *data, uint32_t &offset, uint8_t value);
void unpack(byte *data, uint32_t &offset, uint8_t &value);
void pack(byte *data, uint32_t &offset, uint16_t value);
void unpack(byte *data, uint32_t &offset, uint16_t &value);
void pack(byte *data, uint32_t &offset, uint32_t value);
void unpack(byte *data, uint32_t &offset, uint32_t &value);
void pack(byte *data, uint32_t &offset, uint16_t length, byte *value);
void unpack(byte *data, uint32_t &offset, uint16_t &length, byte *value);
void pack(byte *data, uint32_t &offset, std::string value);
void unpack(byte *data, uint32_t &offset, std::string &value);

#endif // _PACKING_HPP__