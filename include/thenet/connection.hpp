#pragma once

#include <memory>

namespace the
{
namespace net
{
class Socket;

class Connection
{
  public:
    Connection( Socket& );
    const int id;

    typedef std::unique_ptr< Connection > Pointer;
};

}
}

