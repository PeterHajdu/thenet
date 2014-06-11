
#include <igloo/igloo_alt.h>
#include <thenet/socket_pool.hpp>

using namespace igloo;

namespace
{
  void socket_event_callback( the::net::Socket& )
  {
  }

  void read_data_callback( the::net::Socket&, const char*, size_t )
  {
  }
}

Describe(a_socket_pool)
{
  It( is_instantiable )
  {
    the::net::SocketPool pool( &socket_event_callback, &socket_event_callback, &read_data_callback );
  }
};

