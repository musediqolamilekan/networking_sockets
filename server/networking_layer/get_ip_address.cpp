#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

const int SERVER_PORT = 67; 
const int BUFFER_SIZE = 1024;

int main() {
    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_size;

    // Create UDP socket -> SOCK_DGRAM is socket Datagram
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Fill server information
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET; // IPv4
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    serverAddr.sin_port = htons(SERVER_PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for DHCPDISCOVER messages
    addr_size = sizeof(clientAddr); // Initialize size of client address
    
    while (true) {
        std::cout << "Listening for DHCPDISCOVER messages...\n";
        ssize_t len = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&clientAddr, &addr_size);
        buffer[len] = '\0';
        std::cout << "Received message: " << buffer << "\n";

        // Simulate sending a DHCPOFFER message
        const char* offerMessage = "DHCPOFFER: Here is your IP";
        sendto(sockfd, offerMessage, strlen(offerMessage), MSG_CONFIRM, (const struct sockaddr *)&clientAddr, addr_size);
        std::cout << "Sent DHCPOFFER\n";
    }

    close(sockfd);
    return 0;
}
