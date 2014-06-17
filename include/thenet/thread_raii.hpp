#pragma once
#include <thread>

namespace the
{
namespace net
{

class ThreadRaii
{
  public:
    template < typename MainLoopFunctor >
    ThreadRaii( MainLoopFunctor main_loop_functor )
      : m_running( true )
      , m_thread(
          [ this, main_loop_functor ]()
          {
            while( m_running )
            {
              main_loop_functor();
            }
          } )
    {
    }

    ThreadRaii( const ThreadRaii& ) = delete;
    ThreadRaii& operator=( const ThreadRaii& ) = delete;

    ~ThreadRaii()
    {
      m_running = false;
      m_thread.join();
    }

  private:
    volatile bool m_running;
    std::thread m_thread;
};

}
}

