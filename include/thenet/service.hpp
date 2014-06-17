#pragma once
#include <string>
#include <memory>

#include <thenet/connection_pool.hpp>
#include <thenet/socket_pool.hpp>
#include <thenet/thread_raii.hpp>

namespace the
{
namespace net
{

class Service
{
  public:
    Service(
        ConnectionPool::ConnectionEventCallback on_new_connection,
        ConnectionPool::ConnectionEventCallback on_connection_lost );

    ~Service();

    Service( const Service& ) = delete;
    Service& operator=( const Service& ) = delete;

    void start();

    void listen_on( int port );
    void connect_to( const std::string& host, int port );

    template < typename functor >
    void enumerate( functor enumerator );

  private:
    ConnectionPool m_connection_pool;
    SocketPool m_socket_pool;

    std::unique_ptr< ThreadRaii > m_network_thread;
};

template < typename functor >
void Service::enumerate( functor enumerator )
{
  m_connection_pool.enumerate( enumerator );
}

}
}

