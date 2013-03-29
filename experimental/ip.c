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
#define IP_HDR_SIZE 20
#define UDP_HDR_SIZE 8

typedef struct {
    unsigned src:16;
    unsigned dst:16;
    unsigned len:16;
    unsigned csum:16;
} udp_header;

// The order fields within a byte is swapped because the fields are big endian.
typedef struct {
    unsigned ihl:4;
    unsigned version:4;
    unsigned ecn:2;
    unsigned dscp:6;
    unsigned len:16;
    unsigned id:16;
    unsigned offset:13;
    unsigned flags:3;
    unsigned ttl:8;
    unsigned protocol:8;
    unsigned csum:16;
    unsigned src:32;
    unsigned dst:32;
} ip_header;

// A simple test of raw sockets. Creates an IP payload with an IP header, UDP
// header and message and sends it.
//
// Set the DST_IP environment variable to indicate to which IP address the
// packet should be sent.

int main(int argc, char *argv[]) {
    // Make sure everything is the correct size.
    assert(sizeof(udp_header) == UDP_HDR_SIZE);
    assert(sizeof(ip_header) == IP_HDR_SIZE);

    char* dstIP = getenv("DST_IP");
    if (dstIP == NULL) {
        dstIP = DEFAULT_DST_IP;
    }

    int sd = socket(PF_INET, SOCK_RAW, IPPROTO_UDP);

    char* body = "hello";
    int bodyLength = strlen(body);
    int udpLength = UDP_HDR_SIZE + bodyLength;
    int totalLength = IP_HDR_SIZE + udpLength;

    char buf[totalLength];

    ip_header* ip = (ip_header*) &buf;
    ip->version = 4; // IPv4
    ip->ihl = 5; // 5 byte header
    ip->dscp = 0;
    ip->ecn = 0;
    ip->len = totalLength;
    ip->id = 0;
    ip->flags = 2; // Don't fragment
    ip->offset = 0;
    ip->ttl = 64;
    ip->protocol = 17; // UDP
    ip->csum = 0; // Filled in by OS
    ip->src = 0; // Filled in by OS
    ip->dst = inet_addr(dstIP);

    udp_header* header = (udp_header*) &buf[IP_HDR_SIZE];
    header->src = htons(PORT - 2);
    header->dst = htons(PORT);
    header->len = htons(udpLength);
    // The UDP checksum requires the source IP address, which isn't easy to get,
    // so set it to zero, which causes it to be ignored.
    header->csum = 0;

    char* msg = (char*) &buf[IP_HDR_SIZE + UDP_HDR_SIZE];
    strncpy(msg, body, bodyLength);

    struct sockaddr_in dstAddr;
    dstAddr.sin_family = AF_INET;
    dstAddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, dstIP, &dstAddr.sin_addr) != 1) {
        printf("error parsing destination IP address: %s\n", dstIP);
        return 1;
    }

    int one = 1;
    if (setsockopt(sd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) == -1) {
        perror("error setting IP_HDRINCL");
        printf("Are you root?\n");
        return 1;
    }

    if (sendto(sd, buf, totalLength, 0, (struct sockaddr*) &dstAddr, sizeof(dstAddr)) == -1) {
        perror("error sending");
        printf("Are you root?\n");
        return 1;
    }

    return 0;
}
