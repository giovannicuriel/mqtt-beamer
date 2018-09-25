/**
 * @file mqtt.hpp
 * @brief Definitions for MQTT protocol
 * @author Giovanni Curiel dos Santos (giovannicuriel@gmail.com)
 * @version 0.0.1-alpha.1
 * @date 24/09/2018
 * @copyright GPL-3.0
 */
#ifndef _MQTT_HPP__
#define _MQTT_HPP__

#include <stdint.h>
#include "./packing.hpp"

enum MQTTMessageTypes
{
  CONNECT = 0x01,
  CONNECT_ACK = 0x02,
  PUBLISH = 0x03,
  DISCONNECT = 0x0e
};

struct MQTTConnectAck
{
  uint8_t msg_type;
  uint8_t msg_length;
  uint8_t reserved;
  uint8_t result;
  MQTTConnectAck();
  void pack(byte *data, uint32_t &offset);
  void packMessageLength(byte *data, uint32_t &offset);
  void unpack(byte *data, uint32_t &offset);
};

struct MQTTConnectMsg
{
  uint8_t msg_type;
  uint8_t msg_length;
  std::string protocolName;
  uint8_t version;
  uint8_t connect_flags;
  uint16_t keep_alive;
  std::string client_id;

  MQTTConnectMsg();
  void pack(byte *data, uint32_t &offset);
  void packMessageLength(byte *data, uint32_t &offset);
  void unpack(byte *data, uint32_t &offset);
};

struct MQTTPublishMsg
{
  uint8_t msg_type;
  uint8_t msg_length;
  std::string topic;
  std::string message;

  MQTTPublishMsg();
  void pack(byte *data, uint32_t &offset);
  void packMessageLength(byte *data, uint32_t &offset);
  void unpack(byte *data, uint32_t &offset);
};

#endif