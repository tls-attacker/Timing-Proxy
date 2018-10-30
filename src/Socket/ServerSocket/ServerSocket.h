#ifndef ServerSocket_h
#define ServerSocket_h

#include "../Socket.h"
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

namespace Socket {
    class ServerSocket{
        int port;
        int sock;
        //std::string addr;
        int client_sock;
        bool has_client = false;
        struct sockaddr_in6 client_addr;
        socklen_t client_addr_len;

    public:
        ServerSocket() = default;
        ~ServerSocket();
        void bind(int port=0);
        void listen();
        void accept();
        void close_client();
        void close();

        ssize_t read(void* buf, size_t size, bool blocking = true);
        ssize_t write(const void* buf, size_t size);
    };
}

#endif // ServerSocket_h
