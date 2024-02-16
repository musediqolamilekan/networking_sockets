#include "tcp_stream.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>

TCPStream::~TCPStream() {
    close(m_sd);
}

ssize_t TCPStream::send(const char* buffer, size_t len) {
    return write(m_sd, buffer, len);
}

ssize_t TCPStream::receive(char* buffer, size_t len, int timeout) {
    if (timeout <= 0) {
        return read(m_sd, buffer, len);
    }

    if (waitForReadEvent(timeout)) {
        return read(m_sd, buffer, len);
    }
    return connectionTimedOut;
}

std::string TCPStream::getPeerIP() const {
    return m_peerIP;
}

int TCPStream::getPeerPort() const {
    return m_peerPort;
}

bool TCPStream::waitForReadEvent(int timeout) {
    fd_set sdset;
    struct timeval tv;

    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    FD_ZERO(&sdset);
    FD_SET(m_sd, &sdset);
    return select(m_sd + 1, &sdset, nullptr, nullptr, &tv) > 0;
}

TCPStream::TCPStream(int sd, struct sockaddr_in* address) : m_sd(sd) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(address->sin_addr), ip, sizeof(ip));
    m_peerIP = ip;
    m_peerPort = ntohs(address->sin_port);
}
