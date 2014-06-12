#pragma once
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <poll.h>
#include <algorithm>
#include <memory>

#include <thenet/socket.hpp>

namespace the
{
namespace net
{

class SocketPool
{
  public:
    typedef std::function<void(Socket&)> SocketEventCallback;
    typedef std::function<void(Socket&,const char*,size_t)> ReadDataCallback;

    SocketPool( SocketEventCallback new_socket, SocketEventCallback drop_socket, ReadDataCallback read_data );
    void listen( int port );

    //todo: implement address parsing class
    bool connect( const std::string& address, int port );
    void run_for( uint32_t run_for_milliseconds );

  private:
    void add_socket_with_callback( Socket::Pointer&& socket );
    void add_socket( Socket::Pointer&& socket );

    void drop_socket( Socket& socket );
    std::vector<pollfd> m_poll_descriptors;
    std::unordered_map<int, Socket::Pointer> m_sockets;

    SocketEventCallback m_new_socket_callback;
    SocketEventCallback m_drop_socket_callback;
    ReadDataCallback m_read_data_callback;
};

}
}

