#include <iostream>
#include <memory>
#include <cstring>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include "tcp_connector.h"
#include "tcp_stream.h"

std::unique_ptr<TCPStream> TCPConnector::connect(const char *server, int port)
{
    struct sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    if (resolveHostName(server, &(address.sin_addr)) != 0)
    {
        if (inet_pton(AF_INET, server, &(address.sin_addr)) <= 0)
        {
            std::cerr << "Invalid address/ Address not supported" << std::endl;
            return nullptr;
        }
    }

    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0)
    {
        perror("socket() failed");
        return nullptr;
    }

    if (::connect(sd, (struct sockaddr *)&address, sizeof(address)) != 0)
    {
        perror("connect() failed");
        close(sd);
        return nullptr;
    }

    return TCPStream::create(sd, &address);
}

std::unique_ptr<TCPStream> TCPConnector::connect(const char *server, int port, int timeout) {
    if (timeout == 0) {
        return connect(server, port);
    }

    struct sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    if (resolveHostName(server, &(address.sin_addr)) != 0) {
        if (inet_pton(AF_INET, server, &(address.sin_addr)) <= 0) {
            std::cerr << "Invalid address/ Address not supported" << std::endl;
            return nullptr;
        }
    }

    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("socket() failed");
        return nullptr;
    }

    long arg = fcntl(sd, F_GETFL, nullptr);
    arg |= O_NONBLOCK;
    fcntl(sd, F_SETFL, arg);

    if (::connect(sd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        if (errno != EINPROGRESS) {
            perror("connect() failed");
            close(sd);
            return nullptr;
        }
    }

    fd_set sdset;
    struct timeval tv = {timeout, 0};
    FD_ZERO(&sdset);
    FD_SET(sd, &sdset);
    int s = select(sd + 1, nullptr, &sdset, nullptr, &tv);
    if (s < 0) {
        perror("select() error");
        close(sd);
        return nullptr;
    } else if (s == 0) {
        std::cerr << "connect() timed out\n";
        close(sd);
        return nullptr;
    }

    int valopt;
    socklen_t len = sizeof(valopt);
    getsockopt(sd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &len);
    if (valopt) {
        std::cerr << "connect() error " << valopt << " - " << strerror(valopt) << std::endl;
        close(sd);
        return nullptr;
    }

    arg = fcntl(sd, F_GETFL, nullptr);
    arg &= (~O_NONBLOCK);
    fcntl(sd, F_SETFL, arg);

    return TCPStream::create(sd, &address);
}

int TCPConnector::resolveHostName(const char *hostname, struct in_addr *addr)
{
    struct addrinfo hints, *res;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; 

    int result = getaddrinfo(hostname, nullptr, &hints, &res);
    if (result == 0)
    {
        memcpy(addr, &((struct sockaddr_in *)res->ai_addr)->sin_addr, sizeof(struct in_addr));
        freeaddrinfo(res);
    }
    return result;
}
