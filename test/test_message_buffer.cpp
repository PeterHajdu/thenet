#include <igloo/igloo_alt.h>
#include <thenet/message_buffer.hpp>
#include <thenet/types.hpp>

using namespace igloo;

namespace
{
  class MessageParser
  {
    public:
      size_t parse( const char* message, size_t length )
      {
        parse_had_been_called = true;
        message_to_check = std::string( message, length );
        return end_of_message;
      }

      bool parse_had_been_called{ false };
      std::string message_to_check;
      size_t end_of_message{0};
  };

  class MessageChecker
  {
    public:
      void message_ready( the::net::Data&& data )
      {
        passed_message = std::string( begin( data ), end( data ) );
      }

      std::string passed_message;
  };
}

Describe(a_connection_buffer)
{
  void SetUp()
  {
    message_parser.reset( new MessageParser );
    message_checker.reset( new MessageChecker );
    buffer.reset( new the::net::MessageBuffer(
          std::bind(
            &MessageParser::parse,
            message_parser.get(),
            std::placeholders::_1,
            std::placeholders::_2 ),
          std::bind(
            &MessageChecker::message_ready,
            message_checker.get(),
            std::placeholders::_1 ) ) );
  }

  It( is_able_to_receive_data )
  {
    buffer->receive( test_message.c_str(), test_message.length() );
  }

  It( should_ask_message_parser_if_a_message_is_complete )
  {
    buffer->receive( test_message.c_str(), test_message.length() );
    AssertThat( message_parser->parse_had_been_called, Equals( true ) );
  }

  It( should_accumulate_read_data_if_a_message_is_not_complete )
  {
    buffer->receive( test_message.c_str(), test_message.length() );
    AssertThat( message_parser->message_to_check, Equals( test_message ) );

    buffer->receive( test_message.c_str(), test_message.length() );
    AssertThat( message_parser->message_to_check, Equals( test_message + test_message ) );
  }

  It( should_remove_complete_message_from_the_buffer )
  {
    message_parser->end_of_message = test_message.length();
    buffer->receive( test_message.c_str(), test_message.length() );
    buffer->receive( test_message.c_str(), test_message.length() );
    AssertThat( message_parser->message_to_check, Equals( test_message ) );
  }

  It( should_call_message_ready_callback_with_the_complete_message )
  {
    message_parser->end_of_message = test_message.length();
    buffer->receive( test_message.c_str(), test_message.length() );
    AssertThat( message_checker->passed_message, Equals( test_message ) );
  }

  std::unique_ptr< the::net::MessageBuffer > buffer;
  std::unique_ptr< MessageParser > message_parser;
  std::unique_ptr< MessageChecker > message_checker;
  const std::string test_message{ "dog" };
};

