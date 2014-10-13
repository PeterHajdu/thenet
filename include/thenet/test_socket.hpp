#pragma once

#include <thenet/socket.hpp>
#include <thenet/types.hpp>
#include <unistd.h>
#include <fcntl.h>
#include <array>

namespace test
{

class Socket : public the::net::Socket
{
  public:
    typedef std::unique_ptr< test::Socket > Pointer;

    static void set_non_block_on( int fd )
    {
      const int flags( fcntl(fd, F_GETFL, 0) );
      fcntl( fd, F_SETFL, flags | O_NONBLOCK );
    }

    static Pointer create()
    {
      int fds[ 2 ];
      assert( 0 == ::pipe( fds ) );
      set_non_block_on( fds[ 0 ] );
      return Pointer( new Socket( fds[ 1 ], fds[ 0 ] ) );
    }

    ~Socket()
    {
    }

    the::net::Data sent_message()
    {
      const size_t max_size_of_message( 10000 );
      std::array< char, max_size_of_message > buffer;
      const int length_of_message(
          read(
            test_file_descriptor,
            &buffer[ 0 ],
            max_size_of_message ) );

      return length_of_message < 0 && errno == EAGAIN ?
        no_data :
        the::net::Data( begin( buffer ), begin( buffer ) + length_of_message );
    }

    bool has_no_messages()
    {
      return sent_message() == no_data;
    }

    virtual void handle_event() override
    {
    }

    bool was_dropped{ false };
    virtual void drop() override
    {
      was_dropped = true;
    }

  private:
    Socket( int fd, int test_file_descriptor )
      : the::net::Socket( fd )
      , test_file_descriptor( test_file_descriptor )
    {
    }

    const int test_file_descriptor;
    const the::net::Data no_data;
};

}

