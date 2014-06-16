#include <igloo/igloo_alt.h>
#include <thenet/message_queue.hpp>
#include <thenet/types.hpp>

using namespace igloo;

namespace
{
  class LowLevelConnection
  {
    public:
      void send( the::net::Data&& data )
      {
        sent_data.emplace_back( std::move( data ) );
      }


      const the::net::Data last_sent_data()
      {
        return sent_data.empty() ?
          the::net::Data() :
          sent_data.back();
      }


      bool was_data_sent() const
      {
        return !sent_data.empty();
      }


      std::vector< the::net::Data > sent_data;
  };
}

Describe(a_message_queue)
{

  void SetUp()
  {
    low_level_connection.reset( new LowLevelConnection );
    message_queue.reset( new the::net::MessageQueue(
          std::bind(
            &LowLevelConnection::send,
            low_level_connection.get(),
            std::placeholders::_1 ) ) );
  }

  the::net::Data test_data_copy()
  {
    return the::net::Data( test_data );
  }

  void send_test_data()
  {
    AssertThat( message_queue->send( std::move( test_data_copy() ) ), Equals( true ) );
  }

  It( does_not_call_low_level_send_callback_if_it_is_not_waken_up )
  {
    send_test_data();
    AssertThat( low_level_connection->was_data_sent(), Equals( false ) );
  }

  It( forwards_message_to_low_level_callback_when_waken_up )
  {
    send_test_data();
    message_queue->wake_up();
    AssertThat( low_level_connection->last_sent_data(), Equals( test_data ) );
  }

  It( sends_all_data_when_waken_up )
  {
    send_test_data();
    send_test_data();
    message_queue->wake_up();
    AssertThat( low_level_connection->sent_data.size(), Equals( 2u ) );
  }


  bool message_queue_has_test_message()
  {
    the::net::Data buffer;
    return
      message_queue->receive( buffer ) &&
      buffer == test_data;
  }


  It( returns_false_if_there_is_no_message_received )
  {
    AssertThat( message_queue_has_test_message(), Equals( false ) );
  }


  It( returns_all_messages_arrived_from_the_network )
  {
    message_queue->message_from_network( test_data_copy() );
    message_queue->message_from_network( test_data_copy() );
    AssertThat( message_queue_has_test_message(), Equals( true ) );
    AssertThat( message_queue_has_test_message(), Equals( true ) );
  }

  std::unique_ptr< the::net::MessageQueue > message_queue;
  std::unique_ptr< LowLevelConnection > low_level_connection;
  const the::net::Data test_data{{ 't', 'e', 's', 't' }};
  const the::net::Data empty_data;
};
