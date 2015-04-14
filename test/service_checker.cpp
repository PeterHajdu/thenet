#include "service_checker.hpp"
#include <thenet/types.hpp>
#include <thenet/connection.hpp>

namespace test
{

ServiceChecker::ServiceChecker()
  : service(
      std::bind(
        &ServiceChecker::on_new_connection,
        this,
        std::placeholders::_1 ),
      std::bind(
        &ServiceChecker::on_connection_lost,
        this,
        std::placeholders::_1 ) )
{
}


void
ServiceChecker::on_new_connection( the::net::Connection::Pointer connection )
{
  new_connection_was_established = true;
  connections.push_back( connection );
}


void
ServiceChecker::on_connection_lost( the::net::Connection::Pointer connection )
{
  connection_was_lost = true;
}


void
ServiceChecker::send_data( const the::net::Data& data )
{
  connections.back()->send( the::net::Data( data ) );
}


the::net::Data
ServiceChecker::received_data()
{
  the::net::Data message;
  connections.back()->receive( message );
  return message;
}

void
ServiceChecker::drop_connection()
{
  connections.back()->drop();
}

}

