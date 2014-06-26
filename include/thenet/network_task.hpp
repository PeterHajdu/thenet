#pragma once

#include <memory>

namespace the
{
namespace net
{
class Connection;

class NetworkTask
{
  public:
    typedef std::unique_ptr< NetworkTask > Pointer;
    virtual ~NetworkTask() = default;
    virtual void wake_up() = 0;
    virtual void on_message_from_network( const the::net::Data& ) = 0;
};

}
}

