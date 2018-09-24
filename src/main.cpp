#include <iostream>
#include <stdint.h>
#include <string.h>
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <iostream>
#include <sys/socket.h> 
#include <stdio.h> 
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>

typedef uint8_t byte;

enum MQTTMessageTypes {
    CONNECT = 0x01,
    CONNECT_ACK = 0x02,
    PUBLISH = 0x03,
    DISCONNECT = 0x0e
};


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


struct MQTTConnectAck {
    uint8_t msg_type;
    uint8_t msg_length;
    uint8_t reserved;
    uint8_t result;
    MQTTConnectAck() {
        msg_type = CONNECT_ACK;
        msg_length = 0;
        reserved = 0;
        result = 0;
    }

    void pack(byte *data, uint32_t &offset)
    {
        uint8_t value = (msg_type << 4) & 0xf0;
        ::pack(data, offset, value);
        ::pack(data, offset, msg_length);
        ::pack(data, offset, reserved);
        ::pack(data, offset, result);
    };

    void packMessageLength(byte *data, uint32_t &offset) {
        uint8_t value = (msg_type << 4) & 0xf0;
        ::pack(data, offset, value);
        ::pack(data, offset, msg_length);
    }

    void unpack(byte *data, uint32_t &offset)
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

    MQTTConnectMsg() {
        msg_type = CONNECT;
        msg_length = 0;
        protocolName = "MQIsdp";
        version = 3;
        connect_flags = 2;
        keep_alive = 60;
        client_id = "teste";
    }

    void pack(byte *data, uint32_t &offset)
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

    void packMessageLength(byte *data, uint32_t &offset) {
        uint8_t value = (msg_type << 4) & 0xf0;
        ::pack(data, offset, value);
        ::pack(data, offset, msg_length);
    }

    void unpack(byte *data, uint32_t &offset)
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

    MQTTPublishMsg() {
        msg_type = PUBLISH;
        msg_length = 0;
        topic = "";
        message = "";
    }

    void pack(byte *data, uint32_t &offset)
    {
        uint8_t value = (msg_type << 4) & 0xf0;
        ::pack(data, offset, value);
        ::pack(data, offset, msg_length);
        ::pack(data, offset, topic);
        // Remaining bytes is the message
        memcpy((void*)&data[offset], message.c_str(), message.length());
        offset += message.length();
    };

    void packMessageLength(byte *data, uint32_t &offset) {
        uint8_t value = (msg_type << 4) & 0xf0;
        ::pack(data, offset, value);
        ::pack(data, offset, msg_length);
    }

    void unpack(byte *data, uint32_t &offset)
    {
        byte rawMessage[256] = { 0 };
        uint16_t rawSize = 0;
        uint8_t value = 0;
        ::unpack(data, offset, value);
        msg_type = (value >> 4) & 0x0f;
        ::unpack(data, offset, msg_length);
        ::unpack(data, offset, topic);
        rawSize = msg_length - topic.length();
        memcpy(rawMessage, (void*)&data[offset], rawSize);
        message.assign("");
        message.copy((char *)rawMessage, rawSize);
    };
};

int connect() {
    int ret = 0;

    // Creating socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    // Creating server socket address.
    sockaddr_in server;
    memset(&server, 0, sizeof(sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(1883);
    ret = inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);
    if (ret < 0) {
        std::cout << "Error";
        perror("");
        return -1;
    }

    // Connecting to the server
    ret = connect(sock, (struct sockaddr *)&server, sizeof(server));
    if (ret < 0) {
        std::cout << "error while connecting to the server";
        perror("");
        return -1;
    }
    return sock;
}

void sendConnect(int sock) {
    uint16_t totalLength = 0;
    int valread = 0;
    MQTTConnectMsg msg;
    msg.client_id = "admin:d5615d";
    byte data[256] = { 0 };
    uint32_t offset = 0;
    msg.pack(data, offset);
    totalLength = offset;
    msg.msg_length = offset - 2;
    offset = 0;
    msg.packMessageLength(data, offset);
    send(sock, data, totalLength, 0); 

    valread = read( sock , data, 256); 
    printf("%s\n", data); 
}

void sendPublish(int sock, MQTTPublishMsg & publishMsg) {
    
}

int main(void)
{
    int sock = connect();
    sendConnect(sock);
   

    uint16_t totalLength = 0;
    byte data[256] = { 0 };
    uint32_t offset = 0;
    MQTTPublishMsg publishMsg;
    publishMsg.topic = "/admin/d5615d/attrs";
    publishMsg.message = "{\"m\" : \"simple\"}";
    publishMsg.pack(data, offset);
    totalLength = offset;
    publishMsg.msg_length = offset - 2;
    offset = 0;
    publishMsg.packMessageLength(data, offset);


    struct timeval tv1, tv2;
    memset(&tv1, 0, sizeof(struct timeval));
    memset(&tv2, 0, sizeof(struct timeval));
    long int elapsed = 0;
    int freq = 1000;
    long int period = 1.0 / freq * 1000000;
    for (int i = 0; i < 500000; i++) {
        gettimeofday(&tv1, NULL);
        send(sock, data, totalLength, 0);
        gettimeofday(&tv2, NULL);
        // std::cout << "prev: " << tv1.tv_sec << ":" << tv1.tv_usec << ", next: " << tv2.tv_sec << ":" << tv2.tv_usec;
        elapsed = tv2.tv_sec * 1000000 + tv2.tv_usec - (tv1.tv_sec * 1000000 + tv1.tv_usec);
        // std::cout << ", elapsed: " << elapsed << ", period: " << period;
        // std::cout << ", sleep of " << (period - elapsed) << " usec" << std::endl;
        if (period - elapsed > 0)
            usleep(period - elapsed);
        if (i % 50000 == 0) {
            
        }
    }


    return 0;
}
