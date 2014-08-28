#include <igloo/igloo_alt.h>
using namespace igloo;

#include <thenet/service.hpp>
#include <thenet/address.hpp>
#include "test_message.hpp"
#include "service_checker.hpp"

Describe( a_service )
{

  void set_up_server()
  {
    server.reset( new test::ServiceChecker() );
    server->service.listen_on( the::net::Address( test_address ).port );
    server->service.start();
  }

  void set_up_client()
  {
    client.reset( new test::ServiceChecker() );
    client->service.connect_to( test_address );
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

  It( closes_tcp_connection_if_connection_is_dropped )
  {
    client->drop_connection();
    sleep();
    AssertThat( client->connection_was_lost, Equals( true ) );
    AssertThat( server->connection_was_lost, Equals( true ) );
  }

  const test::Message test_message{ "dog food" };
  const std::string test_address{ "localhost:2000" };
  std::unique_ptr< test::ServiceChecker > server;
  std::unique_ptr< test::ServiceChecker > client;
};

