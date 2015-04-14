#pragma once

#include <thenet/types.hpp>
#include <thenet/message_queue.hpp>
#include <thenet/packetizer.hpp>
#include <thenet/network_task.hpp>

#include <memory>

namespace the
{
namespace net
{
class Socket;

class Connection
{
  public:
    typedef std::shared_ptr< Connection > Pointer;

    Connection( Socket& );
    const int id;

    void drop();
    bool send( Data&& );
    bool receive( Data& );

    void send_on_network_thread( Data&& );

    void data_from_network( const char* data, size_t length );
    void message_from_network( Data&& message );
    void wake_up_on_network_thread();

    void register_task( NetworkTask::Pointer&& task );

  private:

    packetizer::Outgoing m_outgoing_packetizer;
    MessageQueue m_message_queue;
    packetizer::Incoming<Connection> m_incoming_packetizer;

    std::vector< NetworkTask::Pointer > m_tasks;

    bool m_should_drop;
    Socket& m_socket;
};

}
}

