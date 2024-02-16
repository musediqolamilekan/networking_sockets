#include <iostream>
#include <cstring>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "tcp_acceptor.h"
#include "tcp_stream.h"

TCPAcceptor::TCPAcceptor(int port, const std::string& address)
    : m_lsd(0), m_port(port), m_address(address), m_listening(false) {}

TCPAcceptor::~TCPAcceptor() {
    if (m_lsd > 0) {
        close(m_lsd);
    }
}

int TCPAcceptor::start() {
    if (m_listening) {
        return 0;
    }

    m_lsd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_lsd < 0) {
        perror("socket() failed");
        return -1;
    }

    struct addrinfo hints, *res;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; 
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; 

    std::string portStr = std::to_string(m_port);
    int result = getaddrinfo(m_address.empty() ? nullptr : m_address.c_str(), portStr.c_str(), &hints, &res);
    if (result != 0) {
        std::cerr << "getaddrinfo() failed for address " << m_address << " with error: " << gai_strerror(result) << std::endl;
        return -1;
    }

    int optval = 1;
    setsockopt(m_lsd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    if (bind(m_lsd, res->ai_addr, res->ai_addrlen) != 0) {
        perror("bind() failed");
        freeaddrinfo(res); // Free memory allocated by getaddrinfo
        return -1;
    }

    freeaddrinfo(res); // Free memory allocated by getaddrinfo

    if (listen(m_lsd, 5) != 0) {
        perror("listen() failed");
        return -1;
    }

    m_listening = true;
    return 0;
}

std::unique_ptr<TCPStream> TCPAcceptor::accept() {
    if (!m_listening) {
        return nullptr;
    }

    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int sd = ::accept(m_lsd, (struct sockaddr*)&addr, &len);
    if (sd < 0) {
        perror("accept() failed");
        return nullptr;
    }

    return TCPStream::create(sd, &addr);
}
