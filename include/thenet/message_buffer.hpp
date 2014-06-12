#pragma once

#include <functional>

#include <thenet/types.hpp>

namespace the
{
namespace net
{

class MessageBuffer
{
  public:
    typedef std::function<size_t( const char*, size_t length )> ParseMessageCallback;
    typedef std::function<void( Data&& message )> MessageCompleteCallback;

    MessageBuffer( ParseMessageCallback, MessageCompleteCallback );
    void receive( const char* buffer, size_t length );

  private:
    const ParseMessageCallback m_parse_message;
    const MessageCompleteCallback m_message_comlete;
    //todo: consider other container, messages are going to be erased from the beginning of the buffer
    Data m_buffer;
};

}
}

