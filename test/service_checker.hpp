#pragma once

#include <thenet/types.hpp>
#include <thenet/service.hpp>

namespace the
{
namespace net
{
class Connection;
}
}

namespace test
{

class ServiceChecker
{
  public:
    ServiceChecker();
    void on_new_connection( the::net::Connection& connection );
    void on_connection_lost( the::net::Connection& connection );
    void send_data( const the::net::Data& data );
    the::net::Data received_data();

    the::net::Service service;
    bool new_connection_was_established{ false };
    bool connection_was_lost{ false };
    std::vector< the::net::Connection* > connections;
};

}

