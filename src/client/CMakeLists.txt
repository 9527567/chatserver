aux_source_directory(. SRC_LIST)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-O2")
add_executable(chatClient ${SRC_LIST})
target_link_libraries(chatClient pthread)