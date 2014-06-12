
set(SOURCE_FILES
  socket_pool.cpp
  message_buffer.cpp
  message_queue.cpp
  socket.cpp
    )

add_library(thenet STATIC ${SOURCE_FILES})
target_link_libraries(thenet ${LIBS})
install(TARGETS thenet DESTINATION lib)

