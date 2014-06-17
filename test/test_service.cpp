#include <igloo/igloo_alt.h>
using namespace igloo;

#include <thenet/service.hpp>
#include "test_message.hpp"

namespace
{
  class ServiceChecker
  {
    public:
      ServiceChecker()
        : service(
            std::bind(
              &ServiceChecker::on_new_connection,
              this,
              std::placeholders::_1 ),
            std::bind(
              &ServiceChecker::on_connection_lost,
              this,
              std::placeholders::_1 ) )
      {
      }

      void on_new_connection( the::net::Connection& connection )
      {
        new_connection_was_established = true;
        connections.push_back( &connection );
      }

      void on_connection_lost( the::net::Connection& connection )
      {
        connection_was_lost = true;
      }

      void send_data( const the::net::Data& data )
      {
        connections.back()->send( the::net::Data( data ) );
      }

      the::net::Data received_data()
      {
        the::net::Data message;
        connections.back()->receive( message );
        return message;
      }

      the::net::Service service;
      bool new_connection_was_established{ false };
      bool connection_was_lost{ false };
      std::vector< the::net::Connection* > connections;
  };
}

Describe( a_service )
{

  void set_up_server()
  {
    server.reset( new ServiceChecker() );
    server->service.listen_on( test_port );
    server->service.start();
  }

  void set_up_client()
  {
    client.reset( new ServiceChecker() );
    client->service.connect_to( test_host, test_port );
    client->service.start();
  }

  void sleep()
  {
    std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );
  }

  void SetUp()
  {
    set_up_server();
    set_up_client();
    sleep();
  }

  It( can_establish_a_connection_on_server_side )
  {
    AssertThat( server->new_connection_was_established, Equals( true ) );
  }

  It( can_establish_a_connection_on_clisnt_side )
  {
    AssertThat( client->new_connection_was_established, Equals( true ) );
  }

  It( can_send_a_message_client_server_direction )
  {
    client->send_data( test_message.plain_data );
    sleep();
    AssertThat( server->received_data(), Equals( test_message.plain_data ) );
  }

  It( can_send_a_message_server_client_direction )
  {
    server->send_data( test_message.plain_data );
    sleep();
    AssertThat( client->received_data(), Equals( test_message.plain_data ) );
  }

  It( can_drop_connection_on_the_server_side )
  {
    client.reset();
    sleep();
    AssertThat( server->connection_was_lost, Equals( true ) );
  }

  It( can_drop_connection_on_the_client_side )
  {
    server.reset();
    sleep();
    AssertThat( client->connection_was_lost, Equals( true ) );
  }

  It( can_enumerate_on_connections )
  {
    std::vector< the::net::Connection* > connections;
    server->service.enumerate(
        [ this, &connections ]( the::net::Connection& connection )
        {
          connections.push_back( &connection );
        });

    AssertThat( connections.empty(), Equals( false ) );
    AssertThat( connections.back(), Equals( server->connections.back() ) );
  }

  const test::Message test_message{ "dog food" };
  const std::string test_host{ "localhost" };
  const int test_port{ 2000 };
  std::unique_ptr< ServiceChecker > server;
  std::unique_ptr< ServiceChecker > client;
};

