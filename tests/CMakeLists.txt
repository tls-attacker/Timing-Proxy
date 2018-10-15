add_subdirectory(googletest-src
        ${CMAKE_BINARY_DIR}/tests/googletest-build
        EXCLUDE_FROM_ALL)

add_subdirectory(PacketParserTest)
add_executable(networkTimingToolTest $<TARGET_OBJECTS:PacketParser> $<TARGET_OBJECTS:PacketParserTest>)
target_link_libraries(networkTimingToolTest gmock_main)
