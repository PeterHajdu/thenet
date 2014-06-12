#pragma once

#include <thenet/types.hpp>
#include <thenet/ring_buffer.hpp>

#include <functional>

namespace the
{
namespace net
{

class MessageQueue
{
  public:
    typedef std::function<size_t(const char *, size_t)> LowLevelSendCallback;
    MessageQueue( LowLevelSendCallback );
    MessageQueue( const MessageQueue& ) = delete;
    MessageQueue& operator=( const MessageQueue& ) = delete;
    bool send( Data&& data );
    bool receive( Data& data );

    void wake_up();

    void message_from_network( Data&& data );

  private:
    static const size_t ring_buffer_size{ 100 };
    RingBuffer<Data, ring_buffer_size> m_out_buffer;
    RingBuffer<Data, ring_buffer_size> m_in_buffer;
    const LowLevelSendCallback m_low_level_send;
};

}
}

