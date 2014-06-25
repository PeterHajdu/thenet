#include <thenet/address.hpp>

#include <igloo/igloo_alt.h>
using namespace igloo;

Describe( an_address )
{
  void SetUp()
  {
    full_tcp_address.reset( new the::net::Address( "tcp://localhost.something:1234" ) );
    address_without_protocol.reset( new the::net::Address( "localhost.something:1234" ) );
  }

  void test_host_of( const the::net::Address& address )
  {
    AssertThat( address.host, Equals( "localhost.something" ) );
  }

  void test_port_of( const the::net::Address& address )
  {
    AssertThat( address.port, Equals( 1234 ) );
  }

  It( provides_host_part_of_address )
  {
    test_host_of( *full_tcp_address );
  }

  It( provides_port_part_of_address )
  {
    test_port_of( *full_tcp_address );
  }

  It( provides_protocol_part_of_address )
  {
    AssertThat( full_tcp_address->protocol, Equals( "tcp" ) );
  }

  It( provides_host_part_of_address_without_protocol )
  {
    test_host_of( *address_without_protocol );
  }

  It( provides_port_part_of_address_without_protocol )
  {
    test_port_of( *address_without_protocol );
  }

  It( provides_protocol_part_of_address_without_protocol )
  {
    AssertThat( address_without_protocol->protocol, Equals( "" ) );
  }

  std::unique_ptr< the::net::Address > full_tcp_address;
  std::unique_ptr< the::net::Address > address_without_protocol;
};

