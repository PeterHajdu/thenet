
set(SOURCE_FILES
  socket_pool.cpp
  message_queue.cpp
  socket.cpp
  connection_pool.cpp
  connection.cpp
    )

add_library(thenet STATIC ${SOURCE_FILES})
target_link_libraries(thenet ${LIBS})
install(TARGETS thenet DESTINATION lib)

