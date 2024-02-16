#include <iostream>
#include <cstdlib> // For atoi and exit
#include <string>
#include "../src/tcp_connector.h"

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "usage: " << argv[0] << " <ip> <port>" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::unique_ptr<TCPConnector> connector = std::make_unique<TCPConnector>();
    auto stream = connector->connect(argv[1], std::atoi(argv[2]));
    if (stream) {
        std::string message = "Is there life on Mars?";
        stream->send(message.c_str(), message.size());
        std::cout << "sent - " << message << std::endl;

        char line[256];
        int len = stream->receive(line, sizeof(line) - 1); 
        if (len > 0) {
            line[len] = '\0'; 
            std::cout << "received - " << line << std::endl;
        } else {
            std::cerr << "Failed to receive message." << std::endl;
        }
    } else {
        std::cerr << "Failed to connect." << std::endl;
    }

    stream = connector->connect(argv[1], std::atoi(argv[2]));
    if (stream) {
        std::string message = "Why is there air?";
        stream->send(message.c_str(), message.size());
        std::cout << "sent - " << message << std::endl;

        char line[256];
        int len = stream->receive(line, sizeof(line) - 1); 
        if (len > 0) {
            line[len] = '\0'; 
            std::cout << "received - " << line << std::endl;
        } else {
            std::cerr << "Failed to receive message." << std::endl;
        }
    } else {
        std::cerr << "Failed to connect." << std::endl;
    }

    return EXIT_SUCCESS;
}
