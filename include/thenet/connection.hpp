#pragma once

#include <thenet/types.hpp>
#include <thenet/message_queue.hpp>

#include <memory>

namespace the
{
namespace net
{
class Socket;

class Connection
{
  public:
    typedef std::unique_ptr< Connection > Pointer;

    Connection( Socket& );
    const int id;

    bool send( Data&& );
    bool receive( Data& );

    void data_from_network( const char* data, size_t length );
    void wake_up_on_network_thread();
  private:
    MessageQueue m_message_queue;
};

}
}

