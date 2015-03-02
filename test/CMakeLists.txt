
# required for igloo:
add_definitions(-Wno-deprecated-declarations)
include_directories(igloo)
include_directories(../src)

set(TEST_SOURCE_FILES
    test_runner.cpp
    test_socket_pool.cpp
    test_socket.cpp
    test_packetizer.cpp
    test_ringbuffer.cpp
    test_message_queue.cpp
    test_connection_pool.cpp
    test_connection.cpp
    test_message.cpp
    test_service.cpp
    test_standalone_connection.cpp
    test_address.cpp
    service_checker.cpp
    )


add_executable(test_runner EXCLUDE_FROM_ALL ${TEST_SOURCE_FILES})
target_link_libraries(test_runner thenet ${LIBS})

get_target_property(TEST_RUNNER_BIN test_runner LOCATION) # for running

find_program( VALGRIND_PATH valgrind )

if(VALGRIND_PATH)
  set(RUNNER
    valgrind --leak-check=full --show-reachable=yes --log-file="valgrind.log"
    )
endif()

add_custom_target(test DEPENDS test_runner)
add_custom_command(TARGET test COMMAND ${RUNNER} ${TEST_RUNNER_BIN} --verbose DEPENDS test_runner)
