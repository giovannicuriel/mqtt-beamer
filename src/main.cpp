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
#include <sstream>

#include "./packing.hpp"
#include "./mqtt.hpp"

int connect(std::string address) {
    int ret = 0;

    // Creating socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    // Creating server socket address.
    sockaddr_in server;
    memset(&server, 0, sizeof(sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(1883);
    ret = inet_pton(AF_INET, address.c_str(), &server.sin_addr);
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

void sendConnect(int sock, std::string tenant, std::string device) {
    uint16_t totalLength = 0;
    int valread = 0;
    MQTTConnectMsg msg;
    msg.client_id = tenant + ":" + device;
    byte data[256] = { 0 };
    uint32_t offset = 0;
    msg.pack(data, offset);
    totalLength = offset;
    msg.msg_length = offset - 2;
    offset = 0;
    msg.packMessageLength(data, offset);
    send(sock, data, totalLength, 0);

    valread = read( sock , data, 256);
}

int main(int argc, char * argv[])
{
    std::stringstream ss;
    std::string address = argv[1];
    std::string tenant = argv[2];
    std::string device = argv[3];
    float rate = atof(argv[4]);
    uint32_t messages = atoi(argv[5]);
    uint16_t totalLength = 0;
    uint32_t offset = 0;
    byte data[256] = { 0 };
    MQTTPublishMsg publishMsg;

    publishMsg.topic = std::string("/") + tenant + "/" + device + "/attrs";

    std::cout << "tenant: " << tenant << ", device: " << device << std::endl;
    std::cout << "freq: " << rate << ", messages: " << messages << std::endl;
    std::cout << "topic: " << publishMsg.topic << std::endl;

    int sock = connect(address);
    sendConnect(sock, tenant, device);

    struct timeval tv1, tv2, startTime, endTime;
    memset(&tv1, 0, sizeof(struct timeval));
    memset(&tv2, 0, sizeof(struct timeval));
    memset(&startTime, 0, sizeof(struct timeval));
    memset(&endTime, 0, sizeof(struct timeval));
    uint32_t elapsed = 0;
    uint32_t freq = rate;
    uint32_t period = 1.0 / freq * 1000000;
    gettimeofday(&startTime, NULL);

    std::string lastMessage = "false";
    for (uint32_t i = 0; i < messages; i++) {
        gettimeofday(&tv1, NULL);
        // Creating packet
        if(i == (messages - 1)){
            lastMessage = "true";
        }
        ss << "{\"perf\": " << tv1.tv_sec * 1000 + tv1.tv_usec / 1000 << ", \"send_order\": "<< (i+1) << ", \"total_messages\": "<< messages << ", \"last_message\": " << lastMessage << "}";
        publishMsg.message = ss.str();
        ss.str("");
        offset = 0;
        publishMsg.pack(data, offset);
        // Updating packet size
        totalLength = offset;
        publishMsg.msg_length = offset - 2;
        offset = 0;
        publishMsg.packMessageLength(data, offset);

        // Send it
        send(sock, data, totalLength, 0);

        // Getting time difference
        gettimeofday(&tv2, NULL);
        elapsed = tv2.tv_sec * 1000000 + tv2.tv_usec - (tv1.tv_sec * 1000000 + tv1.tv_usec);
        if ((int) period - (int) elapsed > 0) {
            usleep(period - elapsed);
        }
    }
    gettimeofday(&endTime, NULL);

    float totalTime = endTime.tv_sec - startTime.tv_sec + (endTime.tv_usec - startTime.tv_usec) / 1000000.0;
    std::cout << "Sent " << messages << " in " << totalTime << ", rate " << messages / totalTime << std::endl;

    return 0;
}
