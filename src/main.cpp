#include <iostream>
#include <stdint.h>
#include <string.h>
#include <string>
#include <arpa/inet.h>
#include <iotstream>

typedef uint8_t byte;

void pack(byte **data, uint32_t &offset, uint8_t value)
{
    memcpy(data[offset], &value, sizeof(uint8_t));
    offset += sizeof(uint8_t);
};
void unpack(byte **data, uint32_t &offset, uint8_t &value)
{
    memcpy(&value, data[offset], sizeof(uint8_t));
    offset += sizeof(uint8_t);
};

void pack(byte **data, uint32_t &offset, uint16_t value)
{
    uint16_t orderedValue = htons(value);
    memcpy(data[offset], &orderedValue, sizeof(uint16_t));
    offset += sizeof(uint16_t);
};
void unpack(byte **data, uint32_t &offset, uint16_t &value)
{
    uint16_t orderedValue = 0;
    memcpy(&orderedValue, data[offset], sizeof(uint16_t));
    offset += sizeof(uint16_t);
    value = ntohs(orderedValue);
};

void pack(byte **data, uint32_t &offset, uint32_t value)
{
    uint32_t orderedValue = htonl(value);
    memcpy(data[offset], &orderedValue, sizeof(uint32_t));
    offset += sizeof(uint32_t);
};
void unpack(byte **data, uint32_t &offset, uint32_t &value)
{
    uint32_t orderedValue = 0;
    memcpy(&orderedValue, data[offset], sizeof(uint32_t));
    value = ntohl(orderedValue);
    offset += sizeof(uint32_t);
};

void pack(byte **data, uint32_t &offset, uint16_t length, byte *value)
{
    ::pack(data, offset, length);
    memcpy(data[offset], value, length);
    offset += length;
};
void unpack(byte **data, uint32_t &offset, uint16_t &length, byte *value)
{
    ::unpack(data, offset, length);
    memcpy(value, data[offset], length);
    offset += length;
};

void pack(byte **data, uint32_t &offset, std::string value)
{
    uint16_t length = value.length();
    ::pack(data, offset, length, (byte *)value.c_str());
}
void unpack(byte **data, uint32_t &offset, std::string &value)
{
    uint16_t length = 0;
    byte rawValue[256] = {0};
    ::unpack(data, offset, length, rawValue);
    value.assign("");
    value.copy((char *)rawValue, length);
}


struct MQTTConnectAck {
    uint8_t msg_type;
    uint8_t msg_length;
    uint8_t reserved;
    uint8_t result;

    void pack(byte **data, uint32_t &offset)
    {
        uint8_t value = (msg_type << 4) & 0xf0;
        ::pack(data, offset, value);
        ::pack(data, offset, msg_length);
        ::pack(data, offset, reserved);
        ::pack(data, offset, result);
    };

    void packMessageLength(byte **data, uint32_t &offset) {
        uint8_t value = (msg_type << 4) & 0xf0;
        ::pack(data, offset, value);
        ::pack(data, offset, msg_length);
    }

    void unpack(byte **data, uint32_t &offset)
    {
        uint8_t value = 0;
        ::unpack(data, offset, value);
        msg_type = (value >> 4) & 0x0f;
        ::unpack(data, offset, msg_length);
        ::pack(data, offset, reserved);
        ::pack(data, offset, result);
    };
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

    void pack(byte **data, uint32_t &offset)
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

    void packMessageLength(byte **data, uint32_t &offset) {
        uint8_t value = (msg_type << 4) & 0xf0;
        ::pack(data, offset, value);
        ::pack(data, offset, msg_length);
    }

    void unpack(byte **data, uint32_t &offset)
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
};


struct MQTTPublishMsg
{
    uint8_t msg_type;
    uint8_t msg_length;
    std::string topic;
    std::string message;

    void pack(byte **data, uint32_t &offset)
    {
        uint8_t value = (msg_type << 4) & 0xf0;
        ::pack(data, offset, value);
        ::pack(data, offset, msg_length);
        ::pack(data, offset, topic);
        // Remaining bytes is the message
        memcpy(data[offset], message.c_str(), message.length() - 1);
        offset += message.length() - 1;
    };

    void packMessageLength(byte **data, uint32_t &offset) {
        uint8_t value = (msg_type << 4) & 0xf0;
        ::pack(data, offset, value);
        ::pack(data, offset, msg_length);
    }

    void unpack(byte **data, uint32_t &offset)
    {
        byte rawMessage[256] = { 0 };
        uint16_t rawSize = 0;
        uint8_t value = 0;
        ::unpack(data, offset, value);
        msg_type = (value >> 4) & 0x0f;
        ::unpack(data, offset, msg_length);
        ::unpack(data, offset, topic);
        rawSize = msg_length - topic.length();
        memcpy(rawMessage, data[offset], rawSize);
        message.assign("");
        message.copy((char *)rawMessage, rawSize);
    };
};

enum MQTTMessageTypes {
    CONNECT = 0x01,
    CONNECT_ACK = 0x02,
    PUBLISH = 0x03,
    DISCONNECT = 0x0e
};

int main(void)
{
    int ret = 0;

    // Creating socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    // Creating server socket address.
    sockaddr_in server;
    memset(&server, 0, sizeof(sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(1883);
    ret = inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);
    if (ret <= 0) {
        std::cout << "Error";
        return -1;
    }

    // Connecting to the server
    ret = connect(sock, (struct sockaddr *)&server, sizeof(server));
    if (ret <= 0) {
        std::cout << "error while connecting to the server";
        return -1;
    }

    MQTTConnectMsg msg;
    byte data[256] = { 0 };
    uint32_t offset = 0;

    msg.pack(data, offset, msg);
    
    send(sock, data, offset, 0); 
    printf("Hello message sent\n"); 
    int valread = read( sock , data, 1024); 
    printf("%s\n",buffer ); 
    return 0;
}
