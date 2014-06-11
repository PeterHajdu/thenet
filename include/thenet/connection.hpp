#pragma once

#include <thenet/types.hpp>
#include <thenet/ring_buffer.hpp>

#include <functional>

namespace the
{
namespace net
{

class Connection
{
  public:
    typedef std::function<void(const char *, size_t)> LowLevelSendCallback;
    Connection( LowLevelSendCallback );
    Connection( const Connection& ) = delete;
    Connection& operator=( const Connection& ) = delete;
    bool send( Data&& data );
    bool receive( Data& data );

    void wake_up();

    void data_from_network( Data&& data );

  private:
    static const size_t ring_buffer_size{ 100 };
    RingBuffer<Data, ring_buffer_size> m_out_buffer;
    RingBuffer<Data, ring_buffer_size> m_in_buffer;
    const LowLevelSendCallback m_low_level_send;
};

}
}

