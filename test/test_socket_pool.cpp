
#include <igloo/igloo_alt.h>
#include <thenet/socket_pool.hpp>
#include <thenet/connection_pool.hpp>

using namespace igloo;

namespace
{
  void connection_event_callback( the::net::Connection& )
  {
  }
}

Describe(a_socket_pool)
{
  It( is_instantiable )
  {
    the::net::ConnectionPool connection_pool( &connection_event_callback, &connection_event_callback );
    the::net::SocketPool pool( connection_pool );
  }
};

