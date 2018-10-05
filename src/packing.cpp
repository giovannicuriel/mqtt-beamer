/**
 * @file packing.cpp
 * @brief Implementation of all functions from packing.hpp header
 * @author Giovanni Curiel dos Santos (giovannicuriel@gmail.com)
 * @version 0.0.1-alpha.1
 * @date 24/09/2018
 * @copyright GPL-3.0
 */
#include "packing.hpp"
#include <string.h>;
#include <arpa/inet.h>

void pack(byte *data, uint32_t &offset, uint8_t value)
{
    memcpy((void*)&data[offset], &value, sizeof(uint8_t));
    offset += sizeof(uint8_t);
};
void unpack(byte *data, uint32_t &offset, uint8_t &value)
{
    memcpy(&value, (void*)&data[offset], sizeof(uint8_t));
    offset += sizeof(uint8_t);
};

void pack(byte *data, uint32_t &offset, uint16_t value)
{
    uint16_t orderedValue = htons(value);
    memcpy((void*)&data[offset], &orderedValue, sizeof(uint16_t));
    offset += sizeof(uint16_t);
};
void unpack(byte *data, uint32_t &offset, uint16_t &value)
{
    uint16_t orderedValue = 0;
    memcpy(&orderedValue, (void*)&data[offset], sizeof(uint16_t));
    offset += sizeof(uint16_t);
    value = ntohs(orderedValue);
};

void pack(byte *data, uint32_t &offset, uint32_t value)
{
    uint32_t orderedValue = htonl(value);
    memcpy((void*)&data[offset], &orderedValue, sizeof(uint32_t));
    offset += sizeof(uint32_t);
};
void unpack(byte *data, uint32_t &offset, uint32_t &value)
{
    uint32_t orderedValue = 0;
    memcpy(&orderedValue, (void*)&data[offset], sizeof(uint32_t));
    value = ntohl(orderedValue);
    offset += sizeof(uint32_t);
};

void pack(byte *data, uint32_t &offset, uint16_t length, byte *value)
{
    ::pack(data, offset, length);
    memcpy((void*)&data[offset], value, length);
    offset += length;
};
void unpack(byte *data, uint32_t &offset, uint16_t &length, byte *value)
{
    ::unpack(data, offset, length);
    memcpy(value, (void*)&data[offset], length);
    offset += length;
};

void pack(byte *data, uint32_t &offset, std::string value)
{
    uint16_t length = value.length();
    ::pack(data, offset, length, (byte *)value.c_str());
}
void unpack(byte *data, uint32_t &offset, std::string &value)
{
    uint16_t length = 0;
    byte rawValue[256] = {0};
    ::unpack(data, offset, length, rawValue);
    value.assign("");
    value.copy((char *)rawValue, length);
}