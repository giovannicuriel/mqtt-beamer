/**
 * @file mqtt.cpp
 * @brief Implementation of all functions from mqtt.hpp header
 * @author Giovanni Curiel dos Santos (giovannicuriel@gmail.com)
 * @version 0.0.1-alpha.1
 * @date 24/09/2018
 * @copyright GPL-3.0
 */
#include "./mqtt.hpp"

MQTTConnectAck::MQTTConnectAck()
{
  msg_type = CONNECT_ACK;
  msg_length = 0;
  reserved = 0;
  result = 0;
}

void MQTTConnectAck::pack(byte *data, uint32_t &offset)
{
  uint8_t value = (msg_type << 4) & 0xf0;
  ::pack(data, offset, value);
  ::pack(data, offset, msg_length);
  ::pack(data, offset, reserved);
  ::pack(data, offset, result);
};

void MQTTConnectAck::packMessageLength(byte *data, uint32_t &offset)
{
  uint8_t value = (msg_type << 4) & 0xf0;
  ::pack(data, offset, value);
  ::pack(data, offset, msg_length);
}

void MQTTConnectAck::unpack(byte *data, uint32_t &offset)
{
  uint8_t value = 0;
  ::unpack(data, offset, value);
  msg_type = (value >> 4) & 0x0f;
  ::unpack(data, offset, msg_length);
  ::pack(data, offset, reserved);
  ::pack(data, offset, result);
};

MQTTConnectMsg::MQTTConnectMsg()
{
  msg_type = CONNECT;
  msg_length = 0;
  protocolName = "MQIsdp";
  version = 3;
  connect_flags = 2;
  keep_alive = 60;
  client_id = "teste";
}

void MQTTConnectMsg::pack(byte *data, uint32_t &offset)
{
  uint8_t value = (msg_type << 4) & 0xf0;
  ::pack(data, offset, value);
  ::pack(data, offset, msg_length);
  ::pack(data, offset, protocolName);
  ::pack(data, offset, version);
  ::pack(data, offset, connect_flags);
  ::pack(data, offset, keep_alive);
  ::pack(data, offset, client_id);
};

void MQTTConnectMsg::packMessageLength(byte *data, uint32_t &offset)
{
  uint8_t value = (msg_type << 4) & 0xf0;
  ::pack(data, offset, value);
  ::pack(data, offset, msg_length);
}

void MQTTConnectMsg::unpack(byte *data, uint32_t &offset)
{
  uint8_t value = 0;
  ::unpack(data, offset, value);
  msg_type = (value >> 4) & 0x0f;
  ::unpack(data, offset, msg_length);
  ::unpack(data, offset, protocolName);
  ::unpack(data, offset, version);
  ::unpack(data, offset, connect_flags);
  ::unpack(data, offset, keep_alive);
  ::unpack(data, offset, client_id);
};

MQTTPublishMsg::MQTTPublishMsg()
{
  msg_type = PUBLISH;
  msg_length = 0;
  topic = "";
  message = "";
}

void MQTTPublishMsg::pack(byte *data, uint32_t &offset)
{
  uint8_t value = (msg_type << 4) & 0xf0;
  ::pack(data, offset, value);
  ::pack(data, offset, msg_length);
  ::pack(data, offset, topic);
  // Remaining bytes is the message
  memcpy((void *)&data[offset], message.c_str(), message.length());
  offset += message.length();
};

void MQTTPublishMsg::packMessageLength(byte *data, uint32_t &offset)
{
  uint8_t value = (msg_type << 4) & 0xf0;
  ::pack(data, offset, value);
  ::pack(data, offset, msg_length);
}

void MQTTPublishMsg::unpack(byte *data, uint32_t &offset)
{
  byte rawMessage[256] = {0};
  uint16_t rawSize = 0;
  uint8_t value = 0;
  ::unpack(data, offset, value);
  msg_type = (value >> 4) & 0x0f;
  ::unpack(data, offset, msg_length);
  ::unpack(data, offset, topic);
  rawSize = msg_length - topic.length();
  memcpy(rawMessage, (void *)&data[offset], rawSize);
  message.assign("");
  message.copy((char *)rawMessage, rawSize);
};
}
;
