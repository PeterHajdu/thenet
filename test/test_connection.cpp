#include <igloo/igloo_alt.h>
using namespace igloo;

#include <thenet/connection.hpp>
#include <thenet/types.hpp>
#include "test_socket.hpp"

Describe( a_connection )
{

  void SetUp()
  {
    socket = test::Socket::create();
    connection.reset( new the::net::Connection( *socket ) );
  }

  It( clones_corresponding_sockets_id )
  {
    AssertThat( socket->id, Equals( connection->id ) );
  }

  the::net::Data copy_of_test_message()
  {
    return the::net::Data( test_message );
  }

  void send_test_data()
  {
    AssertThat( connection->send( copy_of_test_message() ), Equals( true ) );
  }


  It( sends_messages_via_the_corresponding_socket )
  {
    send_test_data();
    connection->wake_up_on_network_thread();
    AssertThat( socket->sent_message(), Equals( test_message ) );
  }

  It( sends_messages_only_when_waken_up_on_the_network_thread )
  {
    send_test_data();
    AssertThat( socket->has_no_messages(), Equals( true ) );
    connection->wake_up_on_network_thread();
    AssertThat( socket->sent_message(), Equals( test_message ) );
  }

  bool connection_has_test_message()
  {
    the::net::Data buffer;
    return connection->receive( buffer ) && buffer == test_message;
  }

  It( returns_all_messages_arrived_from_the_network )
  {
    connection->data_from_network( &test_message[0], test_message.size() );
    AssertThat( connection_has_test_message(), Equals( true ) );
  }

  the::net::Data test_message{ 't', 'e', 's', 't' };
  std::unique_ptr< test::Socket > socket;
  the::net::Connection::Pointer connection;

};

