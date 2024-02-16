#ifndef TCP_CONNECTOR_H 
#define TCP_CONNECTOR_H

#include <netinet/in.h>
#include <memory> 
#include "tcp_stream.h"

class TCPConnector
{
public:
    std::unique_ptr<TCPStream> connect(const char* server, int port);
    std::unique_ptr<TCPStream> connect(const char* server, int port, int timeout);

private:
    int resolveHostName(const char* host, struct in_addr* addr);
};

#endif
