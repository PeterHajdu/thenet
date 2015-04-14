#include <igloo/igloo_alt.h>
using namespace igloo;

#include <thenet/connection_pool.hpp>
#include <thenet/socket.hpp>

#include <thenet/test_socket.hpp>
#include "test_message.hpp"

namespace
{
  class ConnectionPoolChecker
  {
    public:
      void on_new_connection( the::net::Connection::Pointer new_connection )
      {
        on_new_connection_was_called = true;
        new_connections.push_back( new_connection );
      }

      void on_connection_lost( the::net::Connection::Pointer lost_connection )
      {
        on_connection_lost_was_called = true;
        lost_connections.push_back( lost_connection );
      }

      bool on_new_connection_was_called{ false };
      bool on_connection_lost_was_called{ false };
      std::vector< the::net::Connection::Pointer > new_connections;
      std::vector< the::net::Connection::Pointer > lost_connections;
  };
}

Describe( a_connection_pool )
{

  void SetUp()
  {
    sockets.clear();
    checker.reset( new ConnectionPoolChecker() );
    connection_pool.reset(
        new the::net::ConnectionPool(
          std::bind(
            &ConnectionPoolChecker::on_new_connection,
            checker.get(),
            std::placeholders::_1 ),
          std::bind(
            &ConnectionPoolChecker::on_connection_lost,
            checker.get(),
            std::placeholders::_1 ) ) );
    add_new_sockets( 1 );
  }

  void add_new_sockets( size_t count )
  {
    for ( size_t i( 0 ); i < count; ++i )
    {
      sockets.push_back( test::Socket::create() );
      connection_pool->on_new_socket( *sockets.back() );
    }
  }

  It( calls_new_connection_callback_if_a_connection_is_create )
  {
    AssertThat( checker->on_new_connection_was_called, Equals( true ) );
  }

  It( creates_a_connection_for_each_new_socket_created )
  {
    add_new_sockets( 1 );
    AssertThat( checker->new_connections, HasLength( sockets.size() ) );
    AssertThat( checker->new_connections[0].get(), !Equals( checker->new_connections[1].get() ) )
  }

  It( calls_connection_lost_callback_if_a_socket_was_lost )
  {
    connection_pool->on_socket_lost( *sockets[0] );
    AssertThat( checker->on_connection_lost_was_called, Equals( true ) );
  }

  It( calls_connection_lost_callback_if_a_socket_was_lost_with_the_corresponding_connection_object )
  {
    add_new_sockets( 2 );
    the::net::Socket& second_socket( *sockets[1] );
    connection_pool->on_socket_lost( second_socket );

    the::net::Connection::Pointer second_connection( checker->new_connections[ 1 ] );
    AssertThat( checker->lost_connections[0], Equals( second_connection ) );
  }

  It( should_not_delete_the_same_connection_two_times )
  {
    connection_pool->on_socket_lost( *sockets[0] );
    connection_pool->on_socket_lost( *sockets[0] );
    AssertThat( checker->lost_connections, HasLength( 1 ) );
  }

  bool has_test_message( the::net::Connection& connection )
  {
    the::net::Data message;
    return
      connection.receive( message ) &&
      message == test_message.plain_data;
  }

  It( forwards_data_to_connections )
  {
    connection_pool->on_data_available( *sockets[0], test_message.network.data(), test_message.network.size() );
    AssertThat( has_test_message( *checker->new_connections[ 0 ] ), Equals( true ) );
  }

  It( forwards_data_to_the_correct_connection )
  {
    add_new_sockets( 2 );
    the::net::Socket& second_socket( *sockets[1] );
    auto second_connection( checker->new_connections[ 1 ] );

    connection_pool->on_data_available( second_socket, test_message.network.data(), test_message.network.size() );
    AssertThat( has_test_message( *second_connection ), Equals( true ) );
  }

  It( allows_enumeration_on_the_connection_list )
  {
    add_new_sockets( 2 );
    std::vector< the::net::Connection* > connection_list;

    connection_pool->enumerate(
        [ &connection_list ]( the::net::Connection& connection )
        {
          connection_list.push_back( &connection );
        });

    for ( auto connection : checker->new_connections )
    {
      AssertThat( connection_list, Contains( connection.get() ) );
    }
  }

  It( wakes_up_all_connections )
  {
    test::Socket& socket( *sockets[0] );
    the::net::Connection& connection( *checker->new_connections[ 0 ] );
    connection.send( the::net::Data( test_message.plain_data ) );

    connection_pool->wake_up_on_network_thread();
    AssertThat( socket.sent_message(), Equals( test_message.network_data ) );
  }

  std::unique_ptr< ConnectionPoolChecker > checker;
  std::unique_ptr< the::net::ConnectionPool > connection_pool;
  std::vector< test::Socket::Pointer > sockets;
  const test::Message test_message{ "test" };
};

