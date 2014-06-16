
#include <igloo/igloo_alt.h>
#include <packetizer.hpp>
#include <thenet/types.hpp>
#include <netinet/in.h>
#include "test_socket.hpp"

using namespace igloo;

namespace
{
  class UpperLayer
  {
    public:
      void message_from_network( the::net::Data&& data )
      {
        passed_message = std::string( begin( data ), end( data ) );
        was_not_called = false;
      }

      std::string passed_message;
      bool was_not_called{ true };
  };
}

Describe(an_incoming_packetizer)
{
  void set_up_test_message( const std::string& message )
  {
    packetized_test_message = message;
    const uint32_t network_length_of_test_message(
        htonl( packetized_test_message.length() ) );
    network_test_message = std::string(
        reinterpret_cast<const char*>( &network_length_of_test_message ),
        sizeof( network_length_of_test_message ) );
    network_test_message += message;

    const size_t split_at( 5 );
    network_first_part = network_test_message.substr( 0, split_at );
    network_second_part = network_test_message.substr( split_at );
  }

  void SetUp()
  {
    upper_layer.reset( new UpperLayer );
    incoming_packetizer.reset( new the::net::packetizer::Incoming<UpperLayer>( *upper_layer ) );
    set_up_test_message( "dog" );
  }


  It( is_able_to_receive_data )
  {
    incoming_packetizer->receive( network_test_message.c_str(), network_test_message.length() );
  }

  void receive_data( const std::string& data )
  {
    incoming_packetizer->receive( data.c_str(), data.length() );
  }

  It( should_call_message_ready_callback_with_the_complete_message )
  {
    receive_data( network_test_message );
    AssertThat( upper_layer->passed_message, Equals( packetized_test_message ) );
  }

  It( should_accumulate_read_data_if_a_message_is_not_complete )
  {
    receive_data( network_first_part );
    receive_data( network_second_part );
    AssertThat( upper_layer->passed_message, Equals( packetized_test_message ) );
  }

  It( should_call_message_ready_callback_only_if_the_message_is_complete )
  {
    receive_data( network_first_part );
    AssertThat( upper_layer->was_not_called, Equals( true ) );
  }

  It( should_remove_complete_message_from_the_buffer )
  {
    receive_data( network_test_message );
    set_up_test_message( "another test message" );
    receive_data( network_test_message );
    AssertThat( upper_layer->passed_message, Equals( packetized_test_message ) );
  }

  std::unique_ptr< the::net::packetizer::Incoming<UpperLayer> > incoming_packetizer;
  std::unique_ptr< UpperLayer > upper_layer;

  std::string packetized_test_message;
  std::string network_test_message;

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
    return { std::begin( test_message ), std::end( test_message ) };
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
    AssertThat( upper_layer.passed_message, Equals( test_message ) );
  }

  test::Socket::Pointer socket;
  std::string test_message{ "test" };
  std::unique_ptr< the::net::packetizer::Outgoing > outgoing_packetizer;
};


