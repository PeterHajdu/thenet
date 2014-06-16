#include <igloo/igloo_alt.h>
#include <thenet/packetizer.hpp>
#include <thenet/types.hpp>

using namespace igloo;

namespace
{
  class MessageParser
  {
    public:
      static size_t parse( const char* message, size_t length )
      {
        parse_had_been_called = true;
        message_to_check = std::string( message, length );
        return end_of_message;
      }

      static void reset()
      {
        parse_had_been_called = false;
        message_to_check.clear();
        end_of_message = 0;
      }

      static bool parse_had_been_called;
      static std::string message_to_check;
      static size_t end_of_message;
  };

  bool MessageParser::parse_had_been_called( false );
  std::string MessageParser::message_to_check( "" );
  size_t MessageParser::end_of_message( 0 );

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

Describe(a_packetizer)
{
  void SetUp()
  {
    MessageParser::reset();
    upper_layer.reset( new UpperLayer );
    buffer.reset( new the::net::Packetizer<MessageParser,UpperLayer>( *upper_layer ) );
  }

  It( is_able_to_receive_data )
  {
    buffer->receive( test_message.c_str(), test_message.length() );
  }

  It( should_ask_message_parser_if_a_message_is_complete )
  {
    buffer->receive( test_message.c_str(), test_message.length() );
    AssertThat( MessageParser::parse_had_been_called, Equals( true ) );
  }

  It( should_accumulate_read_data_if_a_message_is_not_complete )
  {
    buffer->receive( test_message.c_str(), test_message.length() );
    AssertThat( MessageParser::message_to_check, Equals( test_message ) );

    buffer->receive( test_message.c_str(), test_message.length() );
    AssertThat( MessageParser::message_to_check, Equals( test_message + test_message ) );
  }

  It( should_remove_complete_message_from_the_buffer )
  {
    MessageParser::end_of_message = test_message.length();
    buffer->receive( test_message.c_str(), test_message.length() );
    buffer->receive( test_message.c_str(), test_message.length() );
    AssertThat( MessageParser::message_to_check, Equals( test_message ) );
  }

  It( should_call_message_ready_callback_with_the_complete_message )
  {
    MessageParser::end_of_message = test_message.length();
    buffer->receive( test_message.c_str(), test_message.length() );
    AssertThat( upper_layer->passed_message, Equals( test_message ) );
  }

  It( should_call_message_ready_callback_only_if_the_message_is_complete )
  {
    MessageParser::end_of_message = 0;
    buffer->receive( test_message.c_str(), test_message.length() );
    AssertThat( upper_layer->was_not_called, Equals( true ) );
  }

  std::unique_ptr< the::net::Packetizer<MessageParser,UpperLayer> > buffer;
  std::unique_ptr< UpperLayer > upper_layer;
  const std::string test_message{ "dog" };
};

