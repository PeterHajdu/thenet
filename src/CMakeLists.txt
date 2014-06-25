
set(SOURCE_FILES
  socket_pool.cpp
  message_queue.cpp
  socket.cpp
  connection_pool.cpp
  connection.cpp
  packetizer.cpp
  service.cpp
  socket_utils.cpp
  standalone_connection.cpp
  address.cpp
    )

add_library(thenet STATIC ${SOURCE_FILES})
target_link_libraries(thenet pthread ${LIBS})
install(TARGETS thenet DESTINATION lib)

