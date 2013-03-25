#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>

#define DST_IP "192.168.1.12"
#define PORT 35984
#define UDP_HDR_SIZE 8

struct udp_header {
    short src;
    short dst;
    short len;
    short csum;
};

// A simple test of raw sockets. Creates an IP payload of a UDP packet and
// message and sends it.

int main(int argc, char *argv[]) {
    // Make sure everything is the correct size.
    assert(sizeof(short) == 2);
    assert(sizeof(struct udp_header) == UDP_HDR_SIZE);

    int sd = socket(PF_INET, SOCK_RAW, IPPROTO_UDP);

    char* body = "hello";
    int bodyLength = strlen(body);
    int totalLength = UDP_HDR_SIZE + bodyLength;

    char buf[totalLength];
    struct udp_header* header = (struct udp_header*) &buf;
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
    inet_pton(AF_INET, DST_IP, &dstAddr.sin_addr);

    if (sendto(sd, buf, totalLength, 0, (struct sockaddr*) &dstAddr, sizeof(dstAddr)) == -1) {
        perror("error sending");
        return 1;
    }

    return 0;
}