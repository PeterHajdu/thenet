#include <thenet/connection.hpp>
#include <thenet/types.hpp>

#include <igloo/igloo_alt.h>
using namespace igloo;

#include <netinet/in.h>
#include <thenet/test_socket.hpp>
#include "test_message.hpp"

#include <memory>

namespace
{
  class TestTask : public the::net::NetworkTask
  {
    public:
      bool was_waken_up{ false };
      virtual void wake_up() override
      {
        was_waken_up = true;
      }

      std::string passed_message;
      virtual void on_message_from_network( const the::net::Data& message ) override
      {
        std::string( begin( message ), end( message ) ).swap( passed_message );
      }

  };
}

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
    return the::net::Data( message.plain_data );
  }

  void send_test_data()
  {
    AssertThat( connection->send( copy_of_test_message() ), Equals( true ) );
  }


  It( sends_messages_via_the_corresponding_socket )
  {
    send_test_data();
    connection->wake_up_on_network_thread();
    AssertThat( socket->sent_message(), Equals( message.network_data ) );
  }

  It( sends_messages_only_when_waken_up_on_the_network_thread )
  {
    send_test_data();
    AssertThat( socket->has_no_messages(), Equals( true ) );
    connection->wake_up_on_network_thread();
    AssertThat( socket->sent_message(), Equals( message.network_data ) );
  }

  It( is_possible_to_send_data_from_network_thread )
  {
    connection->send_on_network_thread( copy_of_test_message() );
    AssertThat( socket->sent_message(), Equals( message.network_data ) );
  }

  bool connection_has_test_message()
  {
    the::net::Data buffer;
    return
      connection->receive( buffer ) &&
      buffer == message.plain_data;
  }

  It( returns_all_messages_arrived_from_the_network )
  {
    connection->data_from_network( &message.network[0], message.network.size() );
    AssertThat( connection_has_test_message(), Equals( true ) );
  }

  It( is_possible_to_register_network_tasks_to_a_connection )
  {
    connection->register_task( std::unique_ptr<TestTask>( new TestTask() ) );
  }

  TestTask* register_test_task()
  {
    std::unique_ptr<TestTask> task( new TestTask() );
    TestTask* task_pointer( task.get() );
    connection->register_task( std::move( task ) );
    AssertThat( task_pointer->was_waken_up, Equals( false ) );
    return task_pointer;
  }

  It( wakes_up_all_registered_network_tasks )
  {
    TestTask* first_task( register_test_task() );
    TestTask* second_task( register_test_task() );
    connection->wake_up_on_network_thread();
    AssertThat( first_task->was_waken_up, Equals( true ) );
    AssertThat( second_task->was_waken_up, Equals( true ) );
  }

  It( passes_incoming_messages_to_registered_network_tasks )
  {
    TestTask* first_task( register_test_task() );
    TestTask* second_task( register_test_task() );
    connection->data_from_network( &message.network[0], message.network.size() );
    AssertThat( first_task->passed_message, Equals( message.plain ) );
    AssertThat( second_task->passed_message, Equals( message.plain ) );
  }

  It( drops_corresponding_socket_on_network_thread_after_being_dropped )
  {
    connection->drop();
    AssertThat( socket->was_dropped, Equals( false ) );
    connection->wake_up_on_network_thread();
    AssertThat( socket->was_dropped, Equals( true ) );
  }

  test::Message message{ "test" };
  std::unique_ptr< test::Socket > socket;
  the::net::Connection::Pointer connection;
};

