#ifndef TCPSTREAM_H
#define TCPSTREAM_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <memory>

class TCPStream {
    int m_sd;
    std::string m_peerIP;
    int m_peerPort;

    explicit TCPStream(int sd, struct sockaddr_in* address);

    TCPStream() = delete;
    TCPStream(const TCPStream&) = delete;

    bool waitForReadEvent(int timeout);

public:
    friend class TCPAcceptor;
    friend class TCPConnector;

    virtual ~TCPStream();

    ssize_t send(const char* buffer, size_t len);
    ssize_t receive(char* buffer, size_t len, int timeout = 0);

    std::string getPeerIP() const;
    int getPeerPort() const;

    enum {
        connectionClosed = 0,
        connectionReset = -1,
        connectionTimedOut = -2
    };

    static std::unique_ptr<TCPStream> create(int sd, struct sockaddr_in* address) {
        return std::unique_ptr<TCPStream>(new TCPStream(sd, address));
    }

    TCPStream& operator=(const TCPStream&) = delete;
};

#endif 
