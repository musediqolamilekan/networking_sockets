#ifndef TCPACCEPTOR_H
#define TCPACCEPTOR_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <memory>
#include <string>
#include "tcp_stream.h"

class TCPAcceptor {
    int m_lsd;
    int m_port;
    std::string m_address;
    bool m_listening;

public:
    TCPAcceptor(int port, const std::string& address = "");
    ~TCPAcceptor();

    int start();
    std::unique_ptr<TCPStream> accept();

private:
    TCPAcceptor(const TCPAcceptor& rhs);
    TCPAcceptor& operator=(const TCPAcceptor& rhs);
};

#endif 
