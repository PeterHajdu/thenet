#include <igloo/igloo_alt.h>
using namespace igloo;

#include <thenet/service.hpp>
#include <thenet/standalone_connection.hpp>
#include "test_message.hpp"
#include "service_checker.hpp"

Describe( a_standalone_connection )
{

  void set_up_server()
  {
    server.reset( new test::ServiceChecker() );
    server->service.listen_on( test_port );
    server->service.start();
  }

  void set_up_connection()
  {
    connection.reset( new the::net::StandaloneConnection( test_host, test_port ) );
    connection->connect();
  }

  void sleep()
  {
    std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );
  }

  void SetUp()
  {
    set_up_server();
    set_up_connection();
    sleep();
    AssertThat( server->new_connection_was_established, Equals( true ) );
  }

  It( can_establish_a_connection_with_a_service )
  {
  }

  It( can_send_a_message_to_a_service )
  {
    AssertThat( connection->send( the::net::Data( test_message.plain_data ) ), Equals( true ) );
    sleep();
    AssertThat( server->received_data(), Equals( test_message.plain_data ) );
  }

  It( can_receive_a_message_from_a_service )
  {
    server->send_data( test_message.plain_data );
    sleep();
    the::net::Data buffer;
    AssertThat( connection->receive( buffer ), Equals( true ) );
    AssertThat( buffer, Equals( test_message.plain_data ) );
  }

  const test::Message test_message{ "dog food" };
  const std::string test_host{ "localhost" };
  const int test_port{ 2000 };
  std::unique_ptr< test::ServiceChecker > server;
  std::unique_ptr< the::net::StandaloneConnection > connection;
};

