
#include <igloo/igloo_alt.h>
#include <thenet/socket_pool.hpp>

using namespace igloo;

namespace
{
  void socket_event_callback( yarrr::Socket& )
  {
  }

  void read_data_callback( yarrr::Socket&, const char*, size_t )
  {
  }
}

Describe(a_socket_pool)
{
  It( is_instantiable )
  {
    yarrr::SocketPool pool( &socket_event_callback, &socket_event_callback, &read_data_callback );
  }
};
