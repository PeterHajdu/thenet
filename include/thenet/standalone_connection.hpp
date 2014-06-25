#pragma once

#include <thenet/socket.hpp>
#include <thenet/types.hpp>
#include <thenet/address.hpp>
#include <string>

namespace the
{
namespace net
{
namespace packetizer
{
class Outgoing;
}
class Connection;
class ThreadRaii;
class Address;

class StandaloneConnection
{
  public:
    StandaloneConnection( const Address& address );
    ~StandaloneConnection();

    void connect();
    bool send( Data&& );
    bool receive( Data& );

    void on_socket_lost( Socket& lost_socket );
    void on_data_available( Socket& socket, const char* data, size_t length );
  private:
    const Address m_address;

    Socket::Pointer m_socket;
    std::unique_ptr< Connection > m_connection;
    std::unique_ptr< packetizer::Outgoing > m_packetizer;
    std::unique_ptr< ThreadRaii > m_network_thread;
};

}
}

