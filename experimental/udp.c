#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#define DEFAULT_DST_IP "192.168.1.12"
#define PORT 35984

// A simple test of UDP sockets.
//
// Set the DST_IP environment variable to indicate to which IP address the
// packet should be sent.

int main(int argc, char* argv[]) {
    char* dstIP = getenv("DST_IP");
    if (dstIP == NULL) {
        dstIP = DEFAULT_DST_IP;
    }

    int sd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    int ttl = 64;
    setsockopt(sd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));

    struct sockaddr_in srcAddr;
    srcAddr.sin_family = AF_INET;
    srcAddr.sin_port = htons(PORT - 2);
    srcAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sd, (const struct sockaddr*) &srcAddr, sizeof(srcAddr)) < 0) {
        perror("failed to bind socket");
        return 1;
    }

    struct sockaddr_in dstAddr;
    dstAddr.sin_family = AF_INET;
    dstAddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, dstIP, &dstAddr.sin_addr) != 1) {
        printf("error parsing destination IP address: %s\n", dstIP);
        return 1;
    }

    char* data = "hello";
    int dataLength = strlen(data);

    int sentLength = sendto(sd, data, dataLength, 0, (const struct sockaddr*) &dstAddr, sizeof(dstAddr));
    if (sentLength != dataLength) {
        perror("error sending packet");
        return 1;
    }

    return 0;
}
