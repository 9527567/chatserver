aux_source_directory(. SERVER_SRC)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(chatserver ${SERVER_SRC})
target_link_libraries(chatserver muduo_net muduo_base pthread)