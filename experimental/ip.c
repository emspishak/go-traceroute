#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>

#define DEFAULT_DST_IP "192.168.1.12"
#define PORT 35984
#define UDP_HDR_SIZE 8

typedef struct {
    short src;
    short dst;
    short len;
    short csum;
} udp_header;

// A simple test of raw sockets. Creates an IP payload of a UDP packet and
// message and sends it.
//
// Set the DST_IP environment variable to indicate to which IP address the
// packet should be sent.

int main(int argc, char *argv[]) {
    // Make sure everything is the correct size.
    assert(sizeof(short) == 2);
    assert(sizeof(udp_header) == UDP_HDR_SIZE);

    char* dstIP = getenv("DST_IP");
    if (dstIP == NULL) {
        dstIP = DEFAULT_DST_IP;
    }

    int sd = socket(PF_INET, SOCK_RAW, IPPROTO_UDP);

    char* body = "hello";
    int bodyLength = strlen(body);
    int totalLength = UDP_HDR_SIZE + bodyLength;

    char buf[totalLength];
    udp_header* header = (udp_header*) &buf;
    header->src = htons(PORT - 2);
    header->dst = htons(PORT);
    header->len = htons(totalLength);
    // The UDP checksum requires the source IP address, which isn't easy to get,
    // so set it to zero, which causes it to be ignored.
    header->csum = 0;

    char* msg = (char*) &buf[UDP_HDR_SIZE];
    strncpy(msg, body, bodyLength);

    struct sockaddr_in dstAddr;
    dstAddr.sin_family = AF_INET;
    dstAddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, dstIP, &dstAddr.sin_addr) != 1) {
        printf("error parsing destination IP address: %s\n", dstIP);
        return 1;
    }

    if (sendto(sd, buf, totalLength, 0, (struct sockaddr*) &dstAddr, sizeof(dstAddr)) == -1) {
        perror("error sending");
        printf("Are you root?\n");
        return 1;
    }

    return 0;
}
