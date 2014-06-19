
#include <igloo/igloo_alt.h>
#include <thenet/packetizer.hpp>
#include <thenet/types.hpp>
#include <netinet/in.h>
#include "test_socket.hpp"
#include "test_message.hpp"

using namespace igloo;

namespace
{
  class UpperLayer
  {
    public:
      void message_from_network( the::net::Data&& data )
      {
        passed_messages.push_back( std::string( begin( data ), end( data ) ) );
        was_not_called = false;
      }

      const std::string last_message() const
      {
        return passed_messages.back();
      }

      bool was_not_called{ true };
      std::vector< std::string > passed_messages;
  };
}

Describe(an_incoming_packetizer)
{
  void set_up_test_message( const std::string& message )
  {
    test_message.reset( new test::Message( message ) );
    const size_t split_at( 5 );
    network_first_part = test_message->network.substr( 0, split_at );
    network_second_part = test_message->network.substr( split_at );
  }

  void SetUp()
  {
    upper_layer.reset( new UpperLayer );
    incoming_packetizer.reset( new the::net::packetizer::Incoming<UpperLayer>( *upper_layer ) );
    set_up_test_message( "dog" );
  }


  It( is_able_to_receive_data )
  {
    incoming_packetizer->receive( test_message->network.c_str(), test_message->network.length() );
  }

  void receive_data( const std::string& data )
  {
    incoming_packetizer->receive( data.c_str(), data.length() );
  }

  It( should_call_message_ready_callback_with_the_complete_message )
  {
    receive_data( test_message->network );
    AssertThat( upper_layer->last_message(), Equals( test_message->plain ) );
  }

  It( should_accumulate_read_data_if_a_message_is_not_complete )
  {
    receive_data( network_first_part );
    receive_data( network_second_part );
    AssertThat( upper_layer->last_message(), Equals( test_message->plain ) );
  }

  It( should_call_message_ready_callback_only_if_the_message_is_complete )
  {
    receive_data( network_first_part );
    AssertThat( upper_layer->was_not_called, Equals( true ) );
  }

  It( should_remove_complete_message_from_the_buffer )
  {
    receive_data( test_message->network );
    set_up_test_message( "another test message" );
    receive_data( test_message->network );
    AssertThat( upper_layer->last_message(), Equals( test_message->plain ) );
  }

  It( should_handle_more_packets_in_a_buffer )
  {
    test::Message second_message( "second test message" );
    const std::string two_packets( test_message->network + second_message.network );
    receive_data( two_packets );
    AssertThat( upper_layer->passed_messages.size(), Equals( 2u ) );
    AssertThat( upper_layer->passed_messages[0], Equals( test_message->plain ) );
    AssertThat( upper_layer->passed_messages[1], Equals( second_message.plain ) );
  }

  std::unique_ptr< the::net::packetizer::Incoming<UpperLayer> > incoming_packetizer;
  std::unique_ptr< UpperLayer > upper_layer;

  std::unique_ptr< test::Message > test_message;

  std::string network_first_part;
  std::string network_second_part;
};

Describe(an_outgoing_packetizer)
{
  void SetUp()
  {
    socket = test::Socket::create();
    outgoing_packetizer.reset(
        new the::net::packetizer::Outgoing( *socket ) );
  }

  the::net::Data copy_of_test_message()
  {
    return test_message.plain_data;
  }

  It( sends_messages_over_the_socket )
  {
    outgoing_packetizer->send( copy_of_test_message() );
    AssertThat( socket->has_no_messages(), Equals( false ) );
  }

  It( sends_messages_that_incoming_packetizer_can_parse )
  {
    UpperLayer upper_layer;
    the::net::packetizer::Incoming<UpperLayer> incoming_packetizer( upper_layer );

    outgoing_packetizer->send( copy_of_test_message() );
    const the::net::Data sent_message( socket->sent_message() );

    incoming_packetizer.receive( &sent_message[0], sent_message.size() );
    AssertThat( upper_layer.last_message(), Equals( test_message.plain ) );
  }

  test::Socket::Pointer socket;
  test::Message test_message{ "test" };
  std::unique_ptr< the::net::packetizer::Outgoing > outgoing_packetizer;
};


