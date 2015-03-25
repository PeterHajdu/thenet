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
#include <functional>

#include <thenet/socket.hpp>

namespace the
{
namespace net
{
class ConnectionPool;
class Address;

class SocketPool
{
  public:
    SocketPool( ConnectionPool& connection_pool );
    void listen( int port );

    bool connect( const Address& address );
    void run_for( uint32_t run_for_milliseconds );

    void drop_socket( Socket& socket );
    void on_socket_lost( Socket& socket );
    void on_data_available( Socket&, const char*, size_t );
    void on_new_socket( Socket::Pointer&& socket );
  private:
    ConnectionPool& m_connection_pool;
    void add_socket( Socket::Pointer&& socket );

    std::vector<pollfd> m_poll_descriptors;
    std::unordered_map<int, Socket::Pointer> m_sockets;

    void clean_up_dropped_and_new_sockets();
    std::vector< std::reference_wrapper< Socket > > m_sockets_to_be_dropped;
    std::vector< std::unique_ptr< Socket > > m_sockets_to_be_added;
};

}
}

