#pragma once

#include <thenet/socket.hpp>
#include <thenet/types.hpp>
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

class StandaloneConnection
{
  public:
    StandaloneConnection( const std::string& host, int port );
    ~StandaloneConnection();

    void connect();
    bool send( Data&& );
    bool receive( Data& );

    void on_socket_lost( Socket& lost_socket );
    void on_data_available( Socket& socket, const char* data, size_t length );
  private:
    const std::string m_host;
    const int m_port;

    Socket::Pointer m_socket;
    std::unique_ptr< Connection > m_connection;
    std::unique_ptr< packetizer::Outgoing > m_packetizer;
    std::unique_ptr< ThreadRaii > m_network_thread;
};

}
}

