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

int connect() {
    int ret = 0;

    // Creating socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    // Creating server socket address.
    sockaddr_in server;
    memset(&server, 0, sizeof(sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(8000);
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

#define MSG_SIZE 1024

int main(int argc, char * argv[])
{
    std::stringstream ss;
    std::string tenant = argv[1];
    std::string device = argv[2];
    float rate = atof(argv[3]);
    uint32_t messages = atoi(argv[4]);
    uint16_t totalLength = 0;
    uint32_t offset = 0;
    byte data[MSG_SIZE] = { 0 };
    MQTTPublishMsg publishMsg;

    publishMsg.topic = std::string("/") + tenant + "/" + device + "/attrs";

    std::cout << "tenant: " << tenant << ", device: " << device << std::endl;
    std::cout << "freq: " << rate << ", messages: " << messages << std::endl;
    std::cout << "topic: " << publishMsg.topic << std::endl;

    int sock = connect();
    // sendConnect(sock, tenant, device);

    struct timeval tv1, tv2, startTime, endTime;
    memset(&tv1, 0, sizeof(struct timeval));
    memset(&tv2, 0, sizeof(struct timeval));
    memset(&startTime, 0, sizeof(struct timeval));
    memset(&endTime, 0, sizeof(struct timeval));
    uint32_t elapsed = 0;
    uint32_t freq = rate;
    uint32_t period = 1.0 / freq * 1000000;
    gettimeofday(&startTime, NULL);
    char recvBuf[MSG_SIZE] = { 0 };
    uint32_t recvBuflen = MSG_SIZE;

    char rawHttp[MSG_SIZE] = { 0 };
    totalLength = strlen(rawHttp);
    for (uint32_t i = 0; i < messages; i++) {
        gettimeofday(&tv1, NULL);

        // Send it
        ss << "POST /device HTTP/1.1\r\n"
"Host: localhost:8000\r\n"
"User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:62.0) Gecko/20100101 Firefox/62.0\r\n"
"Accept: */*\r\n"
"Accept-Language: pt-BR,pt;q=0.8,en-US;q=0.5,en;q=0.3\r\n"
"Accept-Encoding: gzip, deflate\r\n"
"Referer: http://localhost:8000/\r\n"
"content-type: application/json\r\n"
"authorization: Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJURmFVTlpxUkluQlY4bEY1ZXprZkE1ck5TdHczUTZRNiIsImlhdCI6MTUzOTAyMzgyMywiZXhwIjoxNTM5MDI0MjQzLCJuYW1lIjoiQWRtaW4gKHN1cGVydXNlcikiLCJlbWFpbCI6ImFkbWluQG5vZW1haWwuY29tIiwicHJvZmlsZSI6ImFkbWluIiwiZ3JvdXBzIjpbMV0sInVzZXJpZCI6MSwianRpIjoiYzM5ZjU5MDNiODA4ZjI2Mzg3ODIwNGQ5NDgxYjg4OTAiLCJzZXJ2aWNlIjoiYWRtaW4iLCJ1c2VybmFtZSI6ImFkbWluIn0.TNyUjUttK3DgUVu9chWhXiLxQdPMSMAc6gSCXL3HXHU\r\n"
"origin: http://localhost:8000\r\n"
"Content-Length: 95\r\n"
"Cookie: csrftoken=zHdRwHXWA7y5O8m0SdHTDT62DyfjCkat; io=D6FgbacifX9r76rUAAAN\r\n"
"DNT: 1\r\n"
"Connection: keep-alive\"\r\n"
"\r\n"
"{\"label\":\"";
        ss << i;
        ss << "\",protocol\":\"MQTT\",\"templates\":[7],\"tags\":[],\"attrs\":[]}\r\n";
        std::cout << "cleaning" << std::endl;
        memset(rawHttp, 0, MSG_SIZE);
        std::cout << "copying" << std::endl;
        memcpy(rawHttp, ss.str().c_str(), ss.str().length());
        totalLength = ss.str().length();
        ss.str("");
        ss.clear();
        std::cout << rawHttp << std::endl;
        std::cout << "sending " << totalLength << " bytes " << std::endl;
        send(sock, rawHttp, totalLength, 0);
        std::cout << "receving" << std::endl;
        recv(sock, recvBuf, recvBuflen, 0);
        std::cout << recvBuf << std::endl;

        // Getting time difference
        gettimeofday(&tv2, NULL);
        elapsed = tv2.tv_sec * 1000000 + tv2.tv_usec - (tv1.tv_sec * 1000000 + tv1.tv_usec);
        if (period - elapsed > 0) {
            usleep(period - elapsed);
        }
    }
    gettimeofday(&endTime, NULL);

    float totalTime = endTime.tv_sec - startTime.tv_sec + (endTime.tv_usec - startTime.tv_usec) / 1000000.0;
    std::cout << "Sent " << messages << " in " << totalTime << ", rate " << messages / totalTime << std::endl;

    return 0;
}
