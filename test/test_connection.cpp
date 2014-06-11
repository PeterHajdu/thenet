#include <igloo/igloo_alt.h>
#include <thenet/connection.hpp>
#include <thenet/types.hpp>

using namespace igloo;

namespace
{
  class LowLevelConnection
  {
    public:
      void send( const char* data, size_t length )
      {
        sent_data.emplace_back( data, data + length );
      }

      const the::net::Data last_sent_data()
      {
        return sent_data.empty() ?
          the::net::Data() :
          sent_data.back();
      }

      std::vector< the::net::Data > sent_data;
  };
}

Describe(a_connection)
{

  void SetUp()
  {
    low_level_connection.reset( new LowLevelConnection );
    connection.reset( new the::net::Connection(
          std::bind(
            &LowLevelConnection::send,
            low_level_connection.get(),
            std::placeholders::_1,
            std::placeholders::_2 ) ) );
  }

  the::net::Data test_data_copy()
  {
    return the::net::Data( test_data );
  }

  It( sends_data )
  {
    AssertThat( connection->send( std::move( test_data_copy() ) ), Equals( true ) );
  }

  It( calls_low_level_send_callback_only_when_waken_up )
  {
    connection->send( std::move( test_data_copy() ) ), Equals( true );
    AssertThat( low_level_connection->last_sent_data(), Equals( empty_data ) );
    connection->wake_up();
    AssertThat( low_level_connection->last_sent_data(), Equals( test_data ) );
  }

  It( sends_all_data_when_waken_up )
  {
    connection->send( std::move( test_data_copy() ) ), Equals( true );
    connection->send( std::move( test_data_copy() ) ), Equals( true );
    connection->wake_up();
    AssertThat( low_level_connection->sent_data.size(), Equals( 2u ) );
  }

  It( returns_false_if_there_is_no_message_received )
  {
    AssertThat( connection->receive( buffer ), Equals( false ) );
  }

  It( returns_all_messages_arrived_from_the_network )
  {
    connection->data_from_network( test_data_copy() );
    connection->data_from_network( test_data_copy() );
    AssertThat( connection->receive( buffer ), Equals( true ) );
    AssertThat( buffer, Equals( test_data ) );
    AssertThat( connection->receive( buffer ), Equals( true ) );
    AssertThat( buffer, Equals( test_data ) );
    AssertThat( connection->receive( buffer ), Equals( false ) );
  }

  std::unique_ptr< the::net::Connection > connection;
  std::unique_ptr< LowLevelConnection > low_level_connection;
  const the::net::Data test_data{{ 't', 'e', 's', 't' }};
  const the::net::Data empty_data;
  the::net::Data buffer;
};
