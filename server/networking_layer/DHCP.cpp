#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>

const int SERVER_PORT = 67;
const int BUFFER_SIZE = 4096; // Increased buffer size for full DHCP packet

// Function to find and return the DHCP Message Type option value
int getDhcpMessageType(const unsigned char* buffer, ssize_t len) {
    if (len < 240 || buffer[0] != 1) { // DHCP messages are BOOTREQUEST type
        return -1; // Invalid or not a DHCP packet
    }

    int index = 240; // DHCP options start after fixed header
    while (index < len) {
        if (buffer[index] == 0xff) { // End option
            break;
        }
        if (buffer[index] == 0) { // Padding
            index++;
            continue;
        }
        if (buffer[index] == 53 && index + 2 < len) { // DHCP Message Type option
            return buffer[index + 2]; // Return the DHCP Message Type
        }
        index += buffer[index + 1] + 2; // Move to the next option
    }

    return -1; // DHCP Message Type option not found
}

int main() {
    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    unsigned char buffer[BUFFER_SIZE];
    socklen_t addr_size;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    if (bind(sockfd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "DHCP Server listening on port " << SERVER_PORT << std::endl;

    while (true) {
        addr_size = sizeof(clientAddr);
        ssize_t len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&clientAddr, &addr_size);
        if (len < 0) {
            perror("recvfrom failed");
            continue; // Handle error but continue server operation
        }

        int messageType = getDhcpMessageType(buffer, len);
        switch (messageType) {
            case 1:
                std::cout << "Received DHCPDISCOVER message." << std::endl;
                break;
            case 3:
                std::cout << "Received DHCPREQUEST message." << std::endl;
                break;
            default:
                std::cout << "Received other DHCP message type: " << messageType << std::endl;
                break;
        }
    }

    close(sockfd);
    return 0;
}
