#include <iostream>
#include <cstdlib>
#include "../src/tcp_acceptor.h"

int main(int argc, char** argv) {
    if (argc < 2 || argc > 3) {
        std::cerr << "usage: server <port> [<ip>]" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::unique_ptr<TCPAcceptor> acceptor;
    if (argc == 3) {
        acceptor = std::make_unique<TCPAcceptor>(std::atoi(argv[1]), argv[2]);
    } else {
        acceptor = std::make_unique<TCPAcceptor>(std::atoi(argv[1]));
    }

    if (acceptor->start() != 0) {
        std::cerr << "Failed to start the acceptor." << std::endl;
        exit(EXIT_FAILURE);
    }

    while (true) {
        auto stream = acceptor->accept();
        if (stream) {
            char line[256];
            ssize_t len;
            while ((len = stream->receive(line, sizeof(line)-1)) > 0) {
                line[len] = '\0';
                std::cout << "received - " << line << std::endl;
                stream->send(line, len);
            }
        }
    }

    return EXIT_SUCCESS; 
}
