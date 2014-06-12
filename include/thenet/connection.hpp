#pragma once

#include <memory>

namespace the
{
namespace net
{

class Connection
{
  public:
    typedef std::unique_ptr< Connection > Pointer;
};

}
}

