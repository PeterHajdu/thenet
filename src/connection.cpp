#include <thenet/connection.hpp>
#include <thenet/socket.hpp>


namespace the
{
namespace net
{

Connection::Connection( Socket& socket )
  : id( socket.id )
{}

}
}

